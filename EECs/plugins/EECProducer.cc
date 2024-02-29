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

#include "SRothman/EECs/src/eec_oo.h"
#include "SRothman/EECs/src/fast.h"
#include "SRothman/EECs/src/fastStructs.h"

#include <iostream>
#include <memory>
#include <vector>

#include <boost/histogram.hpp>
#include <boost/multi_array.hpp>

static arma::mat matFromArray(const boost::multi_array<double, 2>& arr){
    arma::mat mat(arr.shape()[1], arr.shape()[0]);
    for(unsigned i=0; i<arr.shape()[0]; ++i){
        for(unsigned j=0; j<arr.shape()[1]; ++j){
            mat(j, i) = arr[i][j];//NB we transpose
        }
    }
    return mat;
}

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

    void addProjected(EECresult& next, const EECCalculator& calc, 
            bool PU);
    void addResolved(EECresult& next, const EECCalculator& calc, 
            bool PU);
    void addTransfer(EECtransfer& next, const EECCalculator& calc);
};


void EECProducer::addTransfer(EECtransfer& next, 
                              const EECCalculator& calc){

    for(unsigned order=2; order<=calc.getMaxOrder(); ++order){
        next.orders.emplace_back(order);
        next.proj.emplace_back(calc.getTransferproj(order));
    }

    if(doRes3_){
        next.res3 = calc.getTransferres3();
    }
    if(doRes4_){
        next.res4 = calc.getTransferres4();
    }
}

void EECProducer::addProjected(EECresult& next, 
                                const EECCalculator& calc,
                                bool PU){
    for(unsigned order=2; order<=calc.getMaxOrder(); ++order){
        next.orders.emplace_back(order);
        if(PU){
            next.wts.emplace_back(calc.getproj_PU(order));
        } else {
            next.wts.emplace_back(calc.getproj(order));
        }
    }
}

void EECProducer::addResolved(EECresult& next, 
                                const EECCalculator& calc,
                                bool PU){
    if(doRes3_){
        if(PU){
            next.res3wts = calc.getres3_PU();
        } else {
            next.res3wts = calc.getres3();
        }
    }
    if(doRes4_){
        if(PU){
            next.res4wts = calc.getres4_PU();
        } else {
            next.res4wts = calc.getres4();
        }
    }
}

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

      std::cout << "MATCH NAME " << matchTag_ << std::endl;
      std::cout << "GEN NAME " << genTag_ << std::endl;
      std::cout << "RECO NAME " << recoTag_ << std::endl;
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
        
      }

      EECresult next;
      next.iJet = iReco;
      next.iReco = iReco;
      EECresult nextPU;
      nextPU.iJet = iReco;
      nextPU.iReco = iReco;
      if(maxOrder_ >= 2){
          next.orders.push_back(2);
          nextPU.orders.push_back(2);
          next.wts.push_back(ans_reco.wts2);
          nextPU.wts.push_back(ans_reco.wts2_PU);
      }
      if(maxOrder_ >= 3){
          next.orders.push_back(3);
          nextPU.orders.push_back(3);
          next.wts.push_back(ans_reco.wts3);
          nextPU.wts.push_back(ans_reco.wts3_PU);
      }
      if(maxOrder_ >= 4){
          next.orders.push_back(4);
          nextPU.orders.push_back(4);
          next.wts.push_back(ans_reco.wts4);
          nextPU.wts.push_back(ans_reco.wts4_PU);
      }
      if(maxOrder_ >= 5){
          next.orders.push_back(5);
          nextPU.orders.push_back(5);
          next.wts.push_back(ans_reco.wts5);
          nextPU.wts.push_back(ans_reco.wts5_PU);
      }
      if(maxOrder_ >= 6){
          next.orders.push_back(6);
          nextPU.orders.push_back(6);
          next.wts.push_back(ans_reco.wts6);
          nextPU.wts.push_back(ans_reco.wts6_PU);
      }

      if(doRes3_ && maxOrder_ >= 3){

      }

      result->push_back(std::move(next));
      if(verbose_){
        printf("pushed back result\n");
      }

      resultPU->push_back(std::move(nextPU));
      if(verbose_){
        printf("pushed back resultPU\n");
      }
      
      if(iGen >=0 ){
          auto startgen = std::chrono::high_resolution_clock::now();

          fastEEC::result<double> ans_gen;
          const auto& recojet = reco->at(iReco);
          const auto& genjet = gen->at(iGen);
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
        }

        EECresult nextGen;
        nextGen.iJet = iGen;
        nextGen.iReco = iReco;

        EECresult nextGenUNMATCH;
        nextGenUNMATCH.iReco = iReco;
        nextGenUNMATCH.iJet = iGen;

        EECtransfer nextTransfer;
        nextTransfer.iReco = iReco;
        nextTransfer.iGen = iGen;
        
        if(maxOrder_ >= 2){
          nextGen.orders.push_back(2);
          nextGenUNMATCH.orders.push_back(2);
          nextTransfer.orders.push_back(2);
          nextGen.wts.push_back(ans_gen.wts2);
          nextGenUNMATCH.wts.push_back(ans_gen.wts2_PU);
          nextTransfer.proj.push_back(matFromArray(ans_gen.transfer2));
        }
        if(maxOrder_ >= 3){
          nextGen.orders.push_back(3);
          nextGenUNMATCH.orders.push_back(3);
          nextTransfer.orders.push_back(3);
          nextGen.wts.push_back(ans_gen.wts3);
          nextGenUNMATCH.wts.push_back(ans_gen.wts3_PU);
          nextTransfer.proj.push_back(matFromArray(ans_gen.transfer3));
        }
        if(maxOrder_ >= 4){
          nextGen.orders.push_back(4);
          nextGenUNMATCH.orders.push_back(4);
          nextTransfer.orders.push_back(4);
          nextGen.wts.push_back(ans_gen.wts4);
          nextGenUNMATCH.wts.push_back(ans_gen.wts4_PU);
          nextTransfer.proj.push_back(matFromArray(ans_gen.transfer4));
        }
        if(maxOrder_ >= 5){
          nextGen.orders.push_back(5);
          nextGenUNMATCH.orders.push_back(5);
          nextTransfer.orders.push_back(5);
          nextGen.wts.push_back(ans_gen.wts5);
          nextGenUNMATCH.wts.push_back(ans_gen.wts5_PU);
          nextTransfer.proj.push_back(matFromArray(ans_gen.transfer5));
        }
        if(maxOrder_ >= 6){
          nextGen.orders.push_back(6);
          nextGenUNMATCH.orders.push_back(6);
          nextTransfer.orders.push_back(6);
          nextGen.wts.push_back(ans_gen.wts6);
          nextGenUNMATCH.wts.push_back(ans_gen.wts6_PU);
          nextTransfer.proj.push_back(matFromArray(ans_gen.transfer6));
        }


        resultgen->push_back(std::move(nextGen));
        if(verbose_){
          printf("pushed back gen result\n");
        }

        resultUNMATCH->push_back(std::move(nextGenUNMATCH));
        if(verbose_){
          printf("pushed back genUNMATCH result\n");
        }

        resulttrans->push_back(std::move(nextTransfer));
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
