t 1include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "SRothman/DataFormats/interface/jets.h"
#include "SRothman/DataFormats/interface/matching.h"
#include "SRothman/DataFormats/interface/EEC.h"

#include "SRothman/SimonTools/src/deltaR.h"
#include "SRothman/SimonTools/src/util.h"
#include "SRothman/SimonTools/src/recursive_reduce.h"

#include "SRothman/EECs/src/fast.h"
#include "SRothman/EECs/src/fastStructs.h"

#include "SRothman/SimonTools/src/copyMultiArray.h"

#include <iostream>
#include <memory>
#include <vector>

#include <boost/histogram.hpp>
#include <boost/multi_array.hpp>

class EECProducer : public edm::stream::EDProducer<> {
public:
    explicit EECProducer(const edm::ParameterSet&);
    ~EECProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    int verbose_;

    unsigned maxOrder_;
    bool doRes3_, doRes4_, doRes4Fixed_;
    std::string ptNorm_;

    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;

    bool doGen_;
    edm::InputTag genTag_;
    edm::EDGetTokenT<edm::View<jet>> genToken_;
    edm::InputTag matchTag_;
    edm::EDGetTokenT<edm::View<jetmatch>> matchToken_;

    std::vector<double> dRbinEdges_;

    std::vector<double> dRbinEdges_coarse_;

    std::vector<double> xibinEdges_;
    std::vector<double> phibinEdges_;

    std::vector<double> r_dipole_edges_;
    std::vector<double> ct_dipole_edges_;
    
    std::vector<double> r_tee_edges_;
    std::vector<double> ct_tee_edges_;

    std::vector<double> r_triangle_edges_;
    std::vector<double> ct_triangle_edges_;

    double shapetol_;
};


EECProducer::EECProducer(const edm::ParameterSet& conf)
        : verbose_(conf.getParameter<int>("verbose")),
          maxOrder_(conf.getParameter<unsigned>("maxOrder")),
          doRes3_(conf.getParameter<bool>("doRes3")),
          doRes4_(conf.getParameter<bool>("doRes4")),
          doRes4Fixed_(conf.getParameter<bool>("doRes4Fixed")),
          ptNorm_(conf.getParameter<std::string>("ptNorm")),
          recoTag_(conf.getParameter<edm::InputTag>("reco")),
          recoToken_(consumes<edm::View<jet>>(recoTag_)),
          doGen_(conf.getParameter<bool>("doGen")),
          genTag_(conf.getParameter<edm::InputTag>("gen")),
          matchTag_(conf.getParameter<edm::InputTag>("match")),
          dRbinEdges_(conf.getParameter<std::vector<double>>("dRbinEdges")),

          dRbinEdges_coarse_(conf.getParameter<std::vector<double>>("dRbinEdges_coarse")),

          xibinEdges_(conf.getParameter<std::vector<double>>("xibinEdges")),
          phibinEdges_(conf.getParameter<std::vector<double>>("phibinEdges")),

          r_dipole_edges_(conf.getParameter<std::vector<double>>("r_dipole_edges")),
          ct_dipole_edges_(conf.getParameter<std::vector<double>>("ct_dipole_edges")),

          r_tee_edges_(conf.getParameter<std::vector<double>>("r_tee_edges")),
          ct_tee_edges_(conf.getParameter<std::vector<double>>("ct_tee_edges")),

          r_triangle_edges_(conf.getParameter<std::vector<double>>("r_triangle_edges")),
          ct_triangle_edges_(conf.getParameter<std::vector<double>>("ct_triangle_edges")),

          shapetol_(conf.getParameter<double>("shapetol")) {
    produces<std::vector<EECresult>>("reco");
    produces<std::vector<EECresult>>("recoPU");
    if(doGen_){
        produces<std::vector<EECresult>>("gen");
        produces<std::vector<EECresult>>("genUNMATCH");
        produces<std::vector<EECtransfer>>("transfer");
        genToken_ = consumes<edm::View<jet>>(genTag_);
        matchToken_ = consumes<edm::View<jetmatch>>(matchTag_);
    }
}

void EECProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("reco");
  desc.add<edm::InputTag>("gen");
  desc.add<edm::InputTag>("match");

  desc.add<int>("verbose");

  desc.add<unsigned>("maxOrder");
  desc.add<bool>("doRes3");
  desc.add<bool>("doRes4");
  desc.add<bool>("doRes4Fixed");
  desc.add<std::string>("ptNorm");

  desc.add<bool>("doGen");

  desc.add<std::vector<double>>("dRbinEdges");
  desc.add<std::vector<double>>("dRbinEdges_coarse");

  desc.add<std::vector<double>>("xibinEdges");
  desc.add<std::vector<double>>("phibinEdges");

  desc.add<std::vector<double>>("r_dipole_edges");
  desc.add<std::vector<double>>("ct_dipole_edges");

  desc.add<std::vector<double>>("r_tee_edges");
  desc.add<std::vector<double>>("ct_tee_edges");

  desc.add<std::vector<double>>("r_triangle_edges");
  desc.add<std::vector<double>>("ct_triangle_edges");

  desc.add<double>("shapetol");

  descriptions.addWithDefaultLabel(desc);
}

void EECProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  if(verbose_){
    printf("top of EECProducer::produce\n");
  }

  edm::Handle<edm::View<jet>> reco;
  evt.getByToken(recoToken_, reco);

  edm::Handle<edm::View<jet>> gen;
  edm::Handle<edm::View<jetmatch>> matches;
  if(doGen_){
      evt.getByToken(genToken_, gen);
      evt.getByToken(matchToken_, matches);
  }
  if(verbose_){
    printf("got from event\n");
  }

  auto result = std::make_unique<std::vector<EECresult>>();
  auto resultPU = std::make_unique<std::vector<EECresult>>();

  auto resultgen = std::make_unique<std::vector<EECresult>>();
  auto resultUNMATCH = std::make_unique<std::vector<EECresult>>();
  auto resulttrans = std::make_unique<std::vector<EECtransfer>>();

  
  auto RLax = std::make_shared<boost::histogram::axis::variable<double>>(dRbinEdges_);
  auto RLax_coarse = std::make_shared<boost::histogram::axis::variable<double>>(dRbinEdges_coarse_);

  auto xiax = std::make_shared<boost::histogram::axis::variable<double>>(xibinEdges_);
  auto phiax = std::make_shared<boost::histogram::axis::variable<double>>(phibinEdges_);

  auto r_dipole_ax = std::make_shared<boost::histogram::axis::variable<double>>(r_dipole_edges_);
  auto ct_dipole_ax = std::make_shared<boost::histogram::axis::variable<double>>(ct_dipole_edges_);

  auto r_tee_ax = std::make_shared<boost::histogram::axis::variable<double>>(r_tee_edges_);
  auto ct_tee_ax = std::make_shared<boost::histogram::axis::variable<double>>(ct_tee_edges_);

  auto r_triangle_ax = std::make_shared<boost::histogram::axis::variable<double>>(r_triangle_edges_);
  auto ct_triangle_ax = std::make_shared<boost::histogram::axis::variable<double>>(ct_triangle_edges_);

  fastEEC::normType norm;
  if(ptNorm_ == "RAW"){
      norm = fastEEC::normType::RAWPT;
  }else if(ptNorm_ == "CORR"){
      norm = fastEEC::normType::CORRPT;
  }else if(ptNorm_ == "SUM"){
      norm = fastEEC::normType::SUMPT;
  }else{
      throw cms::Exception("Bad norm type");
  }

  for(unsigned iReco=0; iReco<reco->size(); ++iReco){
      if(verbose_){
        printf("iReco %u\n", iReco);
        printf("nRecoPart = %d\n", reco->at(iReco).nPart);
      }

      if(reco->at(iReco).nPart < 2){
          printf("Reco jet with <2 constituents, skipping\n");
          printf("Don't think this should actually ever happen\n");
          throw cms::Exception("Bad reco jet");
          continue;
      }

      int iGen=-1;
      arma::mat ptrans;
      std::vector<bool> PU, UNMATCHED;

      PU.resize(reco->at(iReco).nPart, true);

      if(doGen_){
          int matchidx=-1;
          for(unsigned iM=0; iM<matches->size(); ++iM){
              if(matches->at(iM).iReco == iReco){
                matchidx = iM;
                break;
              }
          }
          if(matchidx>=0){
              iGen = matches->at(matchidx).iGen;
              UNMATCHED.resize(gen->at(iGen).nPart, true);
              ptrans = matches->at(matchidx).ptrans;

              for(unsigned iPReco=0; iPReco<reco->at(iReco).nPart; ++iPReco){
                  for(unsigned iPGen=0; iPGen<gen->at(iGen).nPart; ++iPGen){
                      if(ptrans(iPReco, iPGen) > 0.){
                          PU.at(iPReco) = false;
                          UNMATCHED.at(iPGen) = false;
                      }
                  }
              }
          }
      }

      if(verbose_){
        printf("iGen %d\n", iGen);
      }
      
      auto startreco = std::chrono::high_resolution_clock::now();
      fastEEC::result<double> ans_reco;
      if (doRes3_ && doRes4_ && doRes4Fixed_){
          fastEEC::fastEEC<double, true, false, true, true, true>(
                  ans_reco,
                  reco->at(iReco), RLax, maxOrder_, norm,
                  RLax_coarse, xiax, phiax, 
                  r_dipole_ax, ct_dipole_ax,
                  r_tee_ax, ct_tee_ax,
                  r_triangle_ax, ct_triangle_ax,
                  shapetol_,
                  &PU
          );
      } else if(doRes3_ && doRes4_){
          fastEEC::fastEEC<double, true, false, true, true, false>(
                  ans_reco,
                  reco->at(iReco), RLax, maxOrder_, norm,
                  RLax_coarse, xiax, phiax, 
                  r_dipole_ax, ct_dipole_ax,
                  r_tee_ax, ct_tee_ax,
                  r_triangle_ax, ct_triangle_ax,
                  shapetol_,
                  &PU
          );
      } else if(doRes3_){
          fastEEC::fastEEC<double, true, false, true, false, false>(
                  ans_reco,
                  reco->at(iReco), RLax, maxOrder_, norm,
                  RLax_coarse, xiax, phiax, 
                  r_dipole_ax, ct_dipole_ax,
                  r_tee_ax, ct_tee_ax,
                  r_triangle_ax, ct_triangle_ax,
                  shapetol_,
                  &PU
          );
      } else if(doRes4_ && doRes4Fixed_){
          fastEEC::fastEEC<double, true, false, false, true, true>(
                  ans_reco,
                  reco->at(iReco), RLax, maxOrder_, norm,
                  RLax_coarse, xiax, phiax, 
                  r_dipole_ax, ct_dipole_ax,
                  r_tee_ax, ct_tee_ax,
                  r_triangle_ax, ct_triangle_ax,
                  shapetol_,
                  &PU
          );
      } else if(doRes4_){
          fastEEC::fastEEC<double, true, false, false, true, false>(
                  ans_reco,
                  reco->at(iReco), RLax, maxOrder_, norm,
                  RLax_coarse, xiax, phiax, 
                  r_dipole_ax, ct_dipole_ax,
                  r_tee_ax, ct_tee_ax,
                  r_triangle_ax, ct_triangle_ax,
                  shapetol_,
                  &PU
          );
      } else {
          fastEEC::fastEEC<double, true, false, false, false, false>(
                  ans_reco,
                  reco->at(iReco), RLax, maxOrder_, norm,
                  RLax_coarse, xiax, phiax, 
                  r_dipole_ax, ct_dipole_ax,
                  r_tee_ax, ct_tee_ax,
                  r_triangle_ax, ct_triangle_ax,
                  shapetol_,
                  &PU
          );
      }
      auto endreco = std::chrono::high_resolution_clock::now();
      if(verbose_){
        printf("ran calc\n");
        printf("fast 1: %f\n", std::chrono::duration_cast<std::chrono::microseconds>(endreco - startreco).count() / 1000000.);

        printf("\tsum proj(2): %g\n", recursive_reduce(*ans_reco.wts2, 0.));
        printf("\tsum proj(3): %g\n", recursive_reduce(*ans_reco.wts3, 0.));
        printf("\tsum proj(4): %g\n", recursive_reduce(*ans_reco.wts4, 0.));
        printf("\tsum proj(5): %g\n", recursive_reduce(*ans_reco.wts5, 0.));
        printf("\tsum proj(6): %g\n", recursive_reduce(*ans_reco.wts6, 0.));
        printf("\tsum res3:    %g\n", recursive_reduce(*ans_reco.resolved3, 0.));
        printf("\tsum res4 sh: %g\n", recursive_reduce(*ans_reco.resolved4_shapes, 0.));
        printf("\tsum res4 fi: %g\n", recursive_reduce(*ans_reco.resolved4_fixed, 0.));
        printf("\n");
        printf("\tsum proj(2) PU: %g\n", recursive_reduce(*ans_reco.wts2_PU, 0.));
        printf("\tsum proj(3) PU: %g\n", recursive_reduce(*ans_reco.wts3_PU, 0.));
        printf("\tsum proj(4) PU: %g\n", recursive_reduce(*ans_reco.wts4_PU, 0.));
        printf("\tsum proj(5) PU: %g\n", recursive_reduce(*ans_reco.wts5_PU, 0.));
        printf("\tsum proj(6) PU: %g\n", recursive_reduce(*ans_reco.wts6_PU, 0.));
        printf("\tsum res3 PU:    %g\n", recursive_reduce(*ans_reco.resolved3_PU, 0.));
        printf("\tsum res4 sh PU: %g\n", recursive_reduce(*ans_reco.resolved4_shapes_PU, 0.));
        printf("\tsum res4 fi PU: %g\n", recursive_reduce(*ans_reco.resolved4_fixed_PU, 0.));
        printf("\n");
      }

      result->emplace_back(iReco, iReco, maxOrder_,
                           doRes3_,
                           doRes4_, doRes4Fixed_,
                           ans_reco.wts2, ans_reco.wts3,
                           ans_reco.wts4, ans_reco.wts5,
                           ans_reco.wts6,
                           ans_reco.resolved3,
                           ans_reco.resolved4_shapes,
                           ans_reco.resolved4_fixed);
      if(verbose_){
        printf("pushed back result\n");
      }

      resultPU->emplace_back(iReco, iReco, maxOrder_,
                             doRes3_,
                             doRes4_, doRes4Fixed_,
                             ans_reco.wts2_PU, ans_reco.wts3_PU,
                             ans_reco.wts4_PU, ans_reco.wts5_PU,
                             ans_reco.wts6_PU,
                             ans_reco.resolved3_PU,
                             ans_reco.resolved4_shapes_PU,
                             ans_reco.resolved4_fixed_PU);
      if(verbose_){
        printf("pushed back resultPU\n");
      }
      
      if(iGen >=0 ){
          auto startgen = std::chrono::high_resolution_clock::now();

          fastEEC::result<double> ans_gen;
          const auto& recojet = reco->at(iReco);
          if (doRes3_ && doRes4_ && doRes4Fixed_){
              fastEEC::fastEEC<double, true, true, true, true, true>(
                        ans_gen,
                        gen->at(iGen), RLax, maxOrder_, norm, 
                        RLax_coarse, xiax, phiax,
                        r_dipole_ax, ct_dipole_ax,
                        r_tee_ax, ct_tee_ax,
                        r_triangle_ax, ct_triangle_ax,
                        shapetol_,
                        &UNMATCHED, &recojet, &ptrans
              );
          } else if(doRes3_ && doRes4_){
              fastEEC::fastEEC<double, true, true, true, true, false>(
                        ans_gen,
                        gen->at(iGen), RLax, maxOrder_, norm, 
                        RLax_coarse, xiax, phiax,
                        r_dipole_ax, ct_dipole_ax,
                        r_tee_ax, ct_tee_ax,
                        r_triangle_ax, ct_triangle_ax,
                        shapetol_,
                        &UNMATCHED, &recojet, &ptrans
              );
          } else if(doRes3_){
              fastEEC::fastEEC<double, true, true, true, false, false>(
                        ans_gen,
                        gen->at(iGen), RLax, maxOrder_, norm, 
                        RLax_coarse, xiax, phiax,
                        r_dipole_ax, ct_dipole_ax,
                        r_tee_ax, ct_tee_ax,
                        r_triangle_ax, ct_triangle_ax,
                        shapetol_,
                        &UNMATCHED, &recojet, &ptrans
              );
          } else if(doRes4_ && doRes4Fixed_){
              fastEEC::fastEEC<double, true, true, false, true, true>(
                        ans_gen,
                        gen->at(iGen), RLax, maxOrder_, norm, 
                        RLax_coarse, xiax, phiax,
                        r_dipole_ax, ct_dipole_ax,
                        r_tee_ax, ct_tee_ax,
                        r_triangle_ax, ct_triangle_ax,
                        shapetol_,
                        &UNMATCHED, &recojet, &ptrans
              );
          } else if(doRes4_){
              fastEEC::fastEEC<double, true, true, false, true, false>(
                        ans_gen,
                        gen->at(iGen), RLax, maxOrder_, norm, 
                        RLax_coarse, xiax, phiax,
                        r_dipole_ax, ct_dipole_ax,
                        r_tee_ax, ct_tee_ax,
                        r_triangle_ax, ct_triangle_ax,
                        shapetol_,
                        &UNMATCHED, &recojet, &ptrans
              );
          } else {
              fastEEC::fastEEC<double, true, true, false, false, false>(
                        ans_gen,
                        gen->at(iGen), RLax, maxOrder_, norm, 
                        RLax_coarse, xiax, phiax,
                        r_dipole_ax, ct_dipole_ax,
                        r_tee_ax, ct_tee_ax,
                        r_triangle_ax, ct_triangle_ax,
                        shapetol_,
                        &UNMATCHED, &recojet, &ptrans
              );
          }
          auto endgen = std::chrono::high_resolution_clock::now();

        if(verbose_){
            printf("ran gen calc with %u (gen) x %u (reco) particles\n", gen->at(iGen).nPart, reco->at(iReco).nPart);
            printf("fast gen: %f\n", std::chrono::duration_cast<std::chrono::microseconds>(endgen - startgen).count() / 1000000.);

            printf("\tsum proj(2): %g\n", recursive_reduce(*ans_gen.wts2, 0.));
            printf("\tsum proj(3): %g\n", recursive_reduce(*ans_gen.wts3, 0.));
            printf("\tsum proj(4): %g\n", recursive_reduce(*ans_gen.wts4, 0.));
            printf("\tsum proj(5): %g\n", recursive_reduce(*ans_gen.wts5, 0.));
            printf("\tsum proj(6): %g\n", recursive_reduce(*ans_gen.wts6, 0.));
            printf("\tsum res3:    %g\n", recursive_reduce(*ans_gen.resolved3, 0.));
            printf("\tsum res4 sh: %g\n", recursive_reduce(*ans_gen.resolved4_shapes, 0.));
            printf("\tsum res4 fi: %g\n", recursive_reduce(*ans_gen.resolved4_fixed, 0.));
            printf("\n");
            printf("\tsum proj(2) UM: %g\n", recursive_reduce(*ans_gen.wts2_PU, 0.));
            printf("\tsum proj(3) UM: %g\n", recursive_reduce(*ans_gen.wts3_PU, 0.));
            printf("\tsum proj(4) UM: %g\n", recursive_reduce(*ans_gen.wts4_PU, 0.));
            printf("\tsum proj(5) UM: %g\n", recursive_reduce(*ans_gen.wts5_PU, 0.));
            printf("\tsum proj(6) UM: %g\n", recursive_reduce(*ans_gen.wts6_PU, 0.));
            printf("\tsum res3 UM:    %g\n", recursive_reduce(*ans_gen.resolved3_PU, 0.));
            printf("\tsum res4 sh UM: %g\n", recursive_reduce(*ans_gen.resolved4_shapes_PU, 0.));
            printf("\tsum res4 fi UM: %g\n", recursive_reduce(*ans_gen.resolved4_fixed_PU, 0.));
            printf("\n");
            printf("\tsum proj(2) trans: 1-%g\n", 1-recursive_reduce(*ans_gen.transfer2, 0.));
            printf("\tsum proj(3) trans: 1-%g\n", 1-recursive_reduce(*ans_gen.transfer3, 0.));
            printf("\tsum proj(4) trans: 1-%g\n", 1-recursive_reduce(*ans_gen.transfer4, 0.));
            printf("\tsum proj(5) trans: 1-%g\n", 1-recursive_reduce(*ans_gen.transfer5, 0.));
            printf("\tsum proj(6) trans: 1-%g\n", 1-recursive_reduce(*ans_gen.transfer6, 0.));
            printf("\tsum res3 trans:    1-%g\n", 1-recursive_reduce(*ans_gen.transfer_res3, 0.));
            printf("\tsum res4 sh trans: 1-%g\n", 1-recursive_reduce(*ans_gen.transfer_res4_shapes, 0.));
            printf("\tsum res4 fi trans: 1-%g\n", 1-recursive_reduce(*ans_gen.transfer_res4_fixed, 0.));
        }

        resultgen->emplace_back(iGen, iReco, maxOrder_,
                                doRes3_,
                                doRes4_, doRes4Fixed_,
                                ans_gen.wts2, ans_gen.wts3,
                                ans_gen.wts4, ans_gen.wts5,
                                ans_gen.wts6,
                                ans_gen.resolved3,
                                ans_gen.resolved4_shapes,
                                ans_gen.resolved4_fixed);

        if(verbose_){
          printf("pushed back gen result\n");
        }

        resultUNMATCH->emplace_back(iGen, iReco, maxOrder_,
                                   doRes3_,
                                   doRes4_, doRes4Fixed_,
                                   ans_gen.wts2_PU, ans_gen.wts3_PU,
                                   ans_gen.wts4_PU, ans_gen.wts5_PU,
                                   ans_gen.wts6_PU,
                                   ans_gen.resolved3_PU,
                                   ans_gen.resolved4_shapes_PU,
                                   ans_gen.resolved4_fixed_PU);

        if(verbose_){
          printf("pushed back genUNMATCH result\n");
        }

        resulttrans->emplace_back(iReco, iGen, maxOrder_,
                                  doRes3_,
                                  doRes4_, doRes4Fixed_,
                                  ans_gen.transfer2,
                                  ans_gen.transfer3,
                                  ans_gen.transfer4,
                                  ans_gen.transfer5,
                                  ans_gen.transfer6,
                                  ans_gen.transfer_res3,
                                  ans_gen.transfer_res4_shapes,
                                  ans_gen.transfer_res4_fixed);
        if(verbose_){
          printf("pushed back transfer matrices\n");
        }
      }
  }

  evt.put(std::move(result), "reco");
  evt.put(std::move(resultPU), "recoPU");
  if(doGen_){
      evt.put(std::move(resultgen), "gen");
      evt.put(std::move(resulttrans), "transfer");
      evt.put(std::move(resultUNMATCH), "genUNMATCH");
  }
  if(verbose_){
      printf("put into event\n");
  }
}  // end produce()

DEFINE_FWK_MODULE(EECProducer);
