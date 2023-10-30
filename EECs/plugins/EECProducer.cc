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

    void addProjected(EECresult& next, const ProjectedEECCalculator& calc, bool PU);
    void addResolved(EECresult& next, const ResolvedEECCalculator& calc, bool PU);
    void addNonIRC(EECresult& next, const std::vector<NonIRCEECCalculator>& calc, bool PU);
    void addTransfer(EECtransfer& next, const ProjectedEECCalculator& proj, 
                                        const std::vector<NonIRCEECCalculator>& nonIRC, 
                                        const ResolvedEECCalculator& res,
                                        const std::vector<NonIRCEECCalculator>& nonIRC_reco);
};


void EECProducer::addTransfer(EECtransfer& next, const ProjectedEECCalculator& proj, 
                                        const std::vector<NonIRCEECCalculator>& nonIRC, 
                                        const ResolvedEECCalculator& res,
                                        const std::vector<NonIRCEECCalculator>& nonIRC_reco){

    for(unsigned order=2; order<=proj.getMaxOrder(); ++order){
        next.orders.emplace_back(order);
        next.proj.emplace_back(proj.getTransfer(order));
    }

    for(unsigned i=0; i<nonIRC.size(); ++i){
        int larger = std::max(p1s_[i], p2s_[i]);
        int smaller = std::min(p1s_[i], p2s_[i]);

        next.orders.emplace_back(-10*larger-smaller);

        next.proj.emplace_back(nonIRC[i].getTransfer(2, nonIRC_reco[i]));
    }

    if(doRes3_){
        next.res3 = res.getTransfer(3);
    }
    if(doRes4_){
        next.res4 = res.getTransfer(4);
    }
}

void EECProducer::addProjected(EECresult& next, const ProjectedEECCalculator& calc, bool PU){
    for(unsigned order=2; order<=calc.getMaxOrder(); ++order){
        next.orders.emplace_back(order);
        if(PU){
            next.wts.emplace_back(calc.getwts_PU(order));
        } else {
            next.wts.emplace_back(calc.getwts(order));
        }
        next.covs.emplace_back(calc.getCov(order));
    }
}

void EECProducer::addResolved(EECresult& next, const ResolvedEECCalculator& calc, bool PU){
    if(doRes3_){
        if(PU){
            next.res3wts = calc.getwts_PU(3);
        } else {
            next.res3wts = calc.getwts(3);
        }
        next.cov3 = calc.getCov(3);
    }
    if(doRes4_){
        if(PU){
            next.res4wts = calc.getwts_PU(4);
        } else {
            next.res4wts = calc.getwts(4);
        }
        next.cov4 = calc.getCov(4);
    }
}

void EECProducer::addNonIRC(EECresult& next, const std::vector<NonIRCEECCalculator>& calc, bool PU){
    for(unsigned i=0; i<calc.size(); ++i){
        int larger = std::max(p1s_[i], p2s_[i]);
        int smaller = std::min(p1s_[i], p2s_[i]);

        next.orders.emplace_back(-10*larger-smaller);
        if(PU){
            next.wts.emplace_back(calc[i].getwts_PU(2));
        } else {
            next.wts.emplace_back(calc[i].getwts(2));
        }
        next.covs.emplace_back(calc[i].getCov(2));
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

  boost::histogram::axis::variable<double> dRax(dRbinEdges_);

  for(unsigned iReco=0; iReco<reco->size(); ++iReco){
      if(verbose_){
        printf("iReco %u\n", iReco);
        fflush(stdout);
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
      ProjectedEECCalculator projcalc(verbose_);
      projcalc.setup(reco->at(iReco), maxOrder_, PU, dRax, normToRaw_);

      std::vector<NonIRCEECCalculator> nirccalcs(p1s_.size());
      for(unsigned i=0; i<p1s_.size(); ++i){
          nirccalcs.at(i).setVerbosity(verbose_);
          nirccalcs.at(i).setup(reco->at(iReco), 2u, PU, 
                                p1s_.at(i), p2s_.at(i), dRax, normToRaw_);
      }

      ResolvedEECCalculator rescalc(verbose_);
      if(doRes4_ || doRes3_){
          unsigned resorder = doRes4_ ? 4 : 3;
          rescalc.setup(reco->at(iReco), resorder, PU, dRax, normToRaw_);
      }
      if(verbose_){
        printf("setup reco calculators with %u particles\n", reco->at(iReco).nPart);
      }

      projcalc.run();

      if(verbose_){
        printf("ran projcalc\n");
      }

      for(auto& calc : nirccalcs){
          calc.run();
      }
      if(verbose_){
        printf("ran nirccalcs\n");
      }
            
      if(doRes3_ || doRes4_){
          rescalc.run();
          if(verbose_){
            printf("ran rescalc\n");
          }
      }
      EECresult next;
      next.iJet = iReco;
      next.iReco = iReco;
      addProjected(next, projcalc, false);
      addNonIRC(next, nirccalcs, false);
      addResolved(next, rescalc, false);

      result->push_back(std::move(next));
      if(verbose_){
        printf("pushed back result\n");
      }

      EECresult nextPU;
      nextPU.iJet = iReco;
      nextPU.iReco = iReco;
      addProjected(nextPU, projcalc, true);
      addNonIRC(nextPU, nirccalcs, true);
      addResolved(nextPU, rescalc, true);

      resultPU->push_back(std::move(nextPU));
      if(verbose_){
        printf("pushed back resultPU\n");
      }

      if(iGen >=0 ){
        ProjectedEECCalculator projTcalc(verbose_);
        projTcalc.setup(gen->at(iGen), maxOrder_, UNMATCHED,
                        ptrans, reco->at(iReco), dRax, normToRaw_);

        std::vector<NonIRCEECCalculator> nircTcalcs(p1s_.size());
        for(unsigned i=0; i<p1s_.size(); ++i){
            nircTcalcs.at(i).setup(gen->at(iGen), 2, UNMATCHED,
                                   ptrans, reco->at(iReco),
                                   p1s_.at(i), p2s_.at(i), dRax, normToRaw_);
            nircTcalcs.at(i).setVerbosity(verbose_);
        }

        ResolvedEECCalculator resTcalc(verbose_);
        if(doRes4_ || doRes3_){
            unsigned resorder = doRes4_ ? 4 : 3;
            resTcalc.setup(gen->at(iGen), resorder, UNMATCHED,
                           ptrans, reco->at(iReco), dRax, normToRaw_);
        }

        if(verbose_){
            printf("setup gen calculators with %u (gen) x %u (reco) particles\n", gen->at(iGen).nPart, reco->at(iReco).nPart);
        }

        projTcalc.run();
        if(verbose_){
            printf("ran gen projcalc\n");
        }
        for(auto& calc : nircTcalcs){
            calc.run();
        }
        if(verbose_){
          printf("ran gen nirccalcs\n");
        }
        if(doRes3_ || doRes4_){
            resTcalc.run();
        }
        if(verbose_){
          printf("ran gen rescalc\n");
        }

        EECresult nextGen;
        nextGen.iJet = iGen;
        nextGen.iReco = iReco;
        addProjected(nextGen, projTcalc, false);
        addNonIRC(nextGen, nircTcalcs, false);
        addResolved(nextGen, resTcalc, false);

        resultgen->push_back(std::move(nextGen));
        if(verbose_){
          printf("pushed back gen result\n");
        }

        EECresult nextGenUNMATCH;
        nextGenUNMATCH.iReco = iReco;
        nextGenUNMATCH.iJet = iGen;
        addProjected(nextGenUNMATCH, projTcalc, true);
        addNonIRC(nextGenUNMATCH, nircTcalcs, true);
        addResolved(nextGenUNMATCH, resTcalc, true);

        resultUNMATCH->push_back(std::move(nextGenUNMATCH));
        if(verbose_){
          printf("pushed back genUNMATCH result\n");
        }

        EECtransfer nextTransfer;
        nextTransfer.iReco = iReco;
        nextTransfer.iGen = iGen;
        addTransfer(nextTransfer, projTcalc, nircTcalcs, resTcalc, nirccalcs);

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
