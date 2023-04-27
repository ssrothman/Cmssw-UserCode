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

#include "SRothman/Matching/src/simon_util_cpp/deltaR.h"
#include "SRothman/Matching/src/simon_util_cpp/util.h"

#include "SRothman/EECs/src/eec_oo.h"

#include "SRothman/EECs/plugins/EECutil.h"

#include <iostream>
#include <memory>
#include <vector>

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

    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;

    bool doGen_;
    edm::InputTag genTag_;
    edm::EDGetTokenT<edm::View<jet>> genToken_;
    edm::InputTag matchTag_;
    edm::EDGetTokenT<edm::View<jetmatch>> matchToken_;
};

EECProducer::EECProducer(const edm::ParameterSet& conf)
        : verbose_(conf.getParameter<int>("verbose")),
          maxOrder_(conf.getParameter<unsigned>("maxOrder")),
          p1s_(conf.getParameter<std::vector<unsigned>>("p1s")),
          p2s_(conf.getParameter<std::vector<unsigned>>("p2s")),
          doRes3_(conf.getParameter<bool>("doRes3")),
          doRes4_(conf.getParameter<bool>("doRes4")),
          recoTag_(conf.getParameter<edm::InputTag>("reco")),
          recoToken_(consumes<edm::View<jet>>(recoTag_)),
          doGen_(conf.getParameter<bool>("doGen")),
          genTag_(conf.getParameter<edm::InputTag>("gen")),
          matchTag_(conf.getParameter<edm::InputTag>("match")){
    produces<std::vector<EECresult>>("reco");
    if(doGen_){
        produces<std::vector<EECresult>>("gen");
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
  desc.add<unsigned>("maxOrder");
  descriptions.addWithDefaultLabel(desc);
}

void EECProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<jet>> reco;
  evt.getByToken(recoToken_, reco);

  edm::Handle<edm::View<jet>> gen;
  edm::Handle<edm::View<jetmatch>> matches;
  if(doGen_){
      evt.getByToken(genToken_, gen);
      evt.getByToken(matchToken_, matches);
  }

  auto result = std::make_unique<std::vector<EECresult>>();

  auto resultgen = std::make_unique<std::vector<EECresult>>();
  auto resulttrans = std::make_unique<std::vector<EECtransfer>>();

  for(unsigned iReco=0; iReco<reco->size(); ++iReco){
      int iGen=-1;
      arma::mat ptrans;
      std::vector<bool> PU;

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
              ptrans = matches->at(matchidx).ptrans;

              arma::vec puvec = arma::sum(ptrans, 1);
              for(unsigned i=0; i<reco->at(iReco).nPart; ++i){
                  PU.emplace_back(puvec(i) == 0);
              }
          } else {
              PU.resize(reco->at(iReco).nPart, true);
          }
      }

      ProjectedEECCalculator projcalc;
      projcalc.setup(reco->at(iReco), maxOrder_);

      std::vector<NonIRCEECCalculator<true>> nirccalcs(p1s_.size());
      for(unsigned i=0; i<p1s_.size(); ++i){
          nirccalcs.at(i).setup(reco->at(iReco), 2u, PU, p1s_.at(i), p2s_.at(i));
      }

      ResolvedEECCalculator rescalc;
      if(doRes4_ || doRes3_){
          unsigned resorder = doRes4_ ? 4 : 3;
          rescalc.setup(reco->at(iReco), resorder);
      }

      projcalc.run();

      for(auto& calc : nirccalcs){
          calc.run();
      }
            
      if(doRes3_ || doRes4_){
          rescalc.run();
      }

      EECresult next;
      addEverything(next, projcalc, nirccalcs, rescalc, iReco, reco->at(iReco).nPart);

      result->push_back(std::move(next));

      if(iGen >=0 ){
        ProjectedEECCalculator projTcalc;
        projTcalc.setup(gen->at(iGen), maxOrder_, 
                        ptrans, reco->at(iReco));

        std::vector<NonIRCEECCalculator<false>> nircTcalcs(p1s_.size());
        for(unsigned i=0; i<p1s_.size(); ++i){
            nircTcalcs.at(i).setup(gen->at(iGen), 2,
                                   ptrans, reco->at(iReco),
                                   p1s_.at(i), p2s_.at(i));
        }

        ResolvedEECCalculator resTcalc;
        if(doRes4_ || doRes3_){
            unsigned resorder = doRes4_ ? 4 : 3;
            resTcalc.setup(gen->at(iGen), resorder,
                           ptrans, reco->at(iReco));
        }

        projTcalc.run();
        for(auto& calc : nircTcalcs){
            calc.run();
        }
        if(doRes3_ || doRes4_){
            resTcalc.run();
        }

        EECresult nextGen;
        addEverything(nextGen, projTcalc, nircTcalcs, resTcalc, iGen, gen->at(iGen).nPart);
        resultgen->push_back(std::move(nextGen));

        EECtransfer nextTransfer;
        nextTransfer.iReco = iReco;
        nextTransfer.iGen = iGen;
        for(unsigned order=2; order<maxOrder_; ++order){
            nextTransfer.proj.push_back(projTcalc.getTransfer(order));
            nextTransfer.order.push_back(order);
        }
        for(unsigned i=0; i<p1s_.size(); ++i){
            const auto& gencalc = nircTcalcs.at(i);
            const auto& recocalc = nirccalcs.at(i);
            nextTransfer.proj.push_back(gencalc.getTransfer(2,recocalc));
            nextTransfer.order.push_back(-10*gencalc.getP1()
                                            -gencalc.getP2());
        }

        if(doRes3_ || doRes4_){
            nextTransfer.res3 = resTcalc.getTransfer(3);
            if(doRes4_){
                nextTransfer.res4 = resTcalc.getTransfer(4);
            }
        }
          resulttrans->push_back(std::move(nextTransfer));
      }
  }


  evt.put(std::move(result), "reco");
  if(doGen_){
      evt.put(std::move(resultgen), "gen");
      evt.put(std::move(resulttrans), "transfer");
  }
}  // end produce()

DEFINE_FWK_MODULE(EECProducer);
