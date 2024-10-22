#include "DataFormats/Common/interface/Handle.h"
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

#include "SRothman/SimonTools/src/copyMultiArray.h"

#include "SRothman/EECs/src/theOnlyHeader.h"

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
    bool doTransfer_;
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
          doTransfer_(conf.getParameter<bool>("doTransfer")),
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
  desc.add<bool>("doTransfer");
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
              ptrans = matches->at(matchidx).rawmat;

              for(unsigned iPReco=0; iPReco<reco->at(iReco).nPart; ++iPReco){
                  for(unsigned iPGen=0; iPGen<gen->at(iGen).nPart; ++iPGen){
                      if(ptrans(iPReco, iPGen) > 0.){
                          PU.at(iPReco) = false;
                          UNMATCHED.at(iPGen) = false;
                      }
                  }
              }
          }
            //test ptrans
            /*arma::vec genpt = gen->at(iGen).ptvec();
            arma::vec recpt = reco->at(iReco).ptvec();
            arma::vec fwdpt = ptrans * genpt;
            printf("gen pt: \n");
            std::cout << genpt.t() << std::endl;
            printf("rec pt: \n");
            std::cout << recpt.t() << std::endl;
            printf("fwd pt: \n");
            std::cout << fwdpt.t() << std::endl;
            printf("\n\n");*/
      }

                
      if(verbose_){
        printf("iGen %d\n", iGen);
      }
      
      auto startreco = std::chrono::high_resolution_clock::now();
      fastEEC::result_t<double> ans_reco;
      unsigned flags = 0;
      flags = flags | fastEEC::DOPU;
      if(doRes3_){
          flags = flags | fastEEC::DORES3;
      }
      if(doRes4_){
          flags = flags | fastEEC::DORES4;
      }
      if (doRes4Fixed_){
          //flags = flags | fastEEC::DORES4FIXED;
          throw cms::Exception("Fixed res4 not implemented");
      }
      runFastEEC(
          ans_reco,
          reco->at(iReco), RLax, norm,
          maxOrder_, flags,
          RLax_coarse, xiax, phiax,
          r_dipole_ax, ct_dipole_ax,
          r_tee_ax, ct_tee_ax,
          r_triangle_ax, ct_triangle_ax,
          shapetol_,
          &PU
      );
      auto endreco = std::chrono::high_resolution_clock::now();
      if(verbose_){
        printf("ran calc\n");
        printf("fast 1: %f\n", std::chrono::duration_cast<std::chrono::microseconds>(endreco - startreco).count() / 1000000.);
        ans_reco.summarize();
        fflush(stdout);
      }

      result->emplace_back(iReco, iReco, maxOrder_,
                           doRes3_,
                           doRes4_, doRes4Fixed_,
                           ans_reco.wts,
                           ans_reco.resolved3,
                           ans_reco.resolved4_shapes);
      if(verbose_){
        printf("pushed back result\n");
      }

      resultPU->emplace_back(iReco, iReco, maxOrder_,
                             doRes3_,
                             doRes4_, doRes4Fixed_,
                             ans_reco.wts_PU,
                             ans_reco.resolved3_PU,
                             ans_reco.resolved4_shapes_PU);
      if(verbose_){
        printf("pushed back resultPU\n");
      }
      
      if(iGen >=0 ){
          auto startgen = std::chrono::high_resolution_clock::now();

          if (doTransfer_){
              flags = flags | fastEEC::DOTRANSFER;
          }

          fastEEC::result_t<double> ans_gen;

          runFastEEC(
              ans_gen,
              gen->at(iGen), RLax, norm,
              maxOrder_, flags,
              RLax_coarse, xiax, phiax,
              r_dipole_ax, ct_dipole_ax,
              r_tee_ax, ct_tee_ax,
              r_triangle_ax, ct_triangle_ax,
              shapetol_,
              &UNMATCHED,
              &(reco->at(iReco)),
              &ptrans
          );
          auto endgen = std::chrono::high_resolution_clock::now();

        if(verbose_){
            printf("ran gen calc with %u (gen) x %u (reco) particles\n", gen->at(iGen).nPart, reco->at(iReco).nPart);
            printf("fast gen: %f\n", std::chrono::duration_cast<std::chrono::microseconds>(endgen - startgen).count() / 1000000.);
            ans_gen.summarize();
            fflush(stdout);
        }

        resultgen->emplace_back(iGen, iReco, maxOrder_,
                                doRes3_,
                                doRes4_, doRes4Fixed_,
                                ans_gen.wts,

                                ans_gen.resolved3,
                                ans_gen.resolved4_shapes);

        if(verbose_){
          printf("pushed back gen result\n");
        }

        resultUNMATCH->emplace_back(iGen, iReco, maxOrder_,
                                   doRes3_,
                                   doRes4_, doRes4Fixed_,
                                   ans_gen.wts_PU,
                                   ans_gen.resolved3_PU,
                                   ans_gen.resolved4_shapes_PU);

        if(verbose_){
          printf("pushed back genUNMATCH result\n");
        }

        if (doTransfer_){
            resulttrans->emplace_back(iReco, iGen, 
                                      maxOrder_,
                                      doRes3_,
                                      doRes4_, doRes4Fixed_,
                                      ans_gen.transfer_wts,
                                      ans_gen.transfer_res3,
                                      ans_gen.transfer_res4_shapes);
        }
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
      fflush(stdout);
  }
}  // end produce()

DEFINE_FWK_MODULE(EECProducer);
