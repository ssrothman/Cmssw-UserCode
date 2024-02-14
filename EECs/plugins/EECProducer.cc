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

#include <iostream>
#include <memory>
#include <vector>

#include <boost/histogram.hpp>

class EECProducer : public edm::stream::EDProducer<> {
public:
    explicit EECProducer(const edm::ParameterSet&);
    ~EECProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    int verbose_;

    unsigned maxOrder_;
    std::vector<unsigned> p1s_, p2s_;
    bool doRes3_, doRes4_;
    bool normToRaw_;

    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;

    bool doGen_;
    edm::InputTag genTag_;
    edm::EDGetTokenT<edm::View<jet>> genToken_;
    edm::InputTag matchTag_;
    edm::EDGetTokenT<edm::View<jetmatch>> matchToken_;

    std::vector<double> dRbinEdges_;

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
          p1s_(conf.getParameter<std::vector<unsigned>>("p1s")),
          p2s_(conf.getParameter<std::vector<unsigned>>("p2s")),
          doRes3_(conf.getParameter<bool>("doRes3")),
          doRes4_(conf.getParameter<bool>("doRes4")),
          normToRaw_(conf.getParameter<bool>("normToRaw")),
          recoTag_(conf.getParameter<edm::InputTag>("reco")),
          recoToken_(consumes<edm::View<jet>>(recoTag_)),
          doGen_(conf.getParameter<bool>("doGen")),
          genTag_(conf.getParameter<edm::InputTag>("gen")),
          matchTag_(conf.getParameter<edm::InputTag>("match")),
          dRbinEdges_(conf.getParameter<std::vector<double>>("dRbinEdges")){
    produces<std::vector<EECresult>>("reco");
    produces<std::vector<EECresult>>("recoPU");
    if(doGen_){
        produces<std::vector<EECresult>>("gen");
        produces<std::vector<EECresult>>("genUNMATCH");
        produces<std::vector<EECtransfer>>("transfer");
        genToken_ = consumes<edm::View<jet>>(genTag_);
        matchToken_ = consumes<edm::View<jetmatch>>(matchTag_);
    }
    if (p1s_.size() != p2s_.size()){
        throw cms::Exception("Passed mis-matched p1 and p2 vectors for nonIRC EEC");
    }
}

void EECProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("reco");
  desc.add<edm::InputTag>("gen");
  desc.add<edm::InputTag>("match");
  desc.add<std::vector<unsigned>>("p1s");
  desc.add<std::vector<unsigned>>("p2s");
  desc.add<int>("verbose");
  desc.add<bool>("doGen");
  desc.add<bool>("doRes3");
  desc.add<bool>("doRes4");
  desc.add<bool>("normToRaw");
  desc.add<unsigned>("maxOrder");
  desc.add<std::vector<double>>("dRbinEdges");
  descriptions.addWithDefaultLabel(desc);
}

void EECProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  if(verbose_){
    printf("top of produce\n");
    fflush(stdout);
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
    fflush(stdout);
  }

  auto result = std::make_unique<std::vector<EECresult>>();
  auto resultPU = std::make_unique<std::vector<EECresult>>();

  auto resultgen = std::make_unique<std::vector<EECresult>>();
  auto resultUNMATCH = std::make_unique<std::vector<EECresult>>();
  auto resulttrans = std::make_unique<std::vector<EECtransfer>>();

  auto RLax = std::make_shared<boost::histogram::axis::variable<double>>(dRbinEdges_);

  for(unsigned iReco=0; iReco<reco->size(); ++iReco){
      if(verbose_){
        printf("iReco %u\n", iReco);
        fflush(stdout);
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
      EECCalculator calculator(verbose_);
      calculator.setupProjected(reco->at(iReco), maxOrder_, RLax);
      calculator.addPU(PU);
      if(doRes3_){
          calculator.enableRes3(RLax, RLax);
      }
      if(doRes4_){
          calculator.enableRes4(RLax, RLax);
      }

      calculator.initialize();
      calculator.run();

      if(verbose_){
        printf("ran calc\n");
      }

      EECresult next;
      next.iJet = iReco;
      next.iReco = iReco;
      addProjected(next, calculator, false);
      addResolved(next, calculator, false);

      result->push_back(std::move(next));
      if(verbose_){
        printf("pushed back result\n");
      }

      EECresult nextPU;
      nextPU.iJet = iReco;
      nextPU.iReco = iReco;
      addProjected(nextPU, calculator, true);
      addResolved(nextPU, calculator, true);

      resultPU->push_back(std::move(nextPU));
      if(verbose_){
        printf("pushed back resultPU\n");
      }

      if(iGen >=0 ){
          EECCalculator Tcalc(verbose_);
          Tcalc.setupProjected(gen->at(iGen), maxOrder_, RLax);
          Tcalc.addPU(UNMATCHED);
          Tcalc.addTransfer(gen->at(iGen), reco->at(iReco), ptrans);
          if(doRes3_){
              Tcalc.enableRes3(RLax, RLax);
          }
          if(doRes4_){
              Tcalc.enableRes4(RLax, RLax);
          }

          Tcalc.initialize();

        if(verbose_){
            printf("setup gen calculators with %u (gen) x %u (reco) particles\n", gen->at(iGen).nPart, reco->at(iReco).nPart);
        }

        Tcalc.run();
        if(verbose_){
            printf("ran gen calc\n");
        }

        EECresult nextGen;
        nextGen.iJet = iGen;
        nextGen.iReco = iReco;
        addProjected(nextGen, Tcalc, false);
        addResolved(nextGen, Tcalc, false);

        resultgen->push_back(std::move(nextGen));
        if(verbose_){
          printf("pushed back gen result\n");
        }

        EECresult nextGenUNMATCH;
        nextGenUNMATCH.iReco = iReco;
        nextGenUNMATCH.iJet = iGen;
        addProjected(nextGenUNMATCH, Tcalc, true);
        addResolved(nextGenUNMATCH, Tcalc, true);

        resultUNMATCH->push_back(std::move(nextGenUNMATCH));
        if(verbose_){
          printf("pushed back genUNMATCH result\n");
        }

        EECtransfer nextTransfer;
        nextTransfer.iReco = iReco;
        nextTransfer.iGen = iGen;
        addTransfer(nextTransfer, Tcalc);

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
