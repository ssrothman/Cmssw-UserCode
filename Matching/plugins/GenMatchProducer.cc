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

#include "SRothman/CustomJets/src/ParticleUncertainty.h"

#include "SRothman/DataFormats/interface/jets.h"
#include "SRothman/DataFormats/interface/matching.h"

#include "SRothman/Matching/src/simon_util_cpp/deltaR.h"
#include "SRothman/Matching/src/simon_util_cpp/util.h"

#include "SRothman/Matching/src/matcher.h"

#include <iostream>
#include <memory>
#include <vector>

bool hasNearby(const jet& source, const reco::Candidate& cand){
    for(const auto& part : source.particles){
        double dist = dR2(part.eta, part.phi,
                          cand.eta(), cand.phi());
        if(dist < 0.2*0.2){
            return true;
        }
    }
    return false;
}

void makeNearbyJet(jet& result, const jet& source, 
                   const edm::Handle<edm::View<reco::Candidate>>& candidates,
                   bool doUncertainty){
    for(const auto& cand : *candidates){
        if(cand.pt() <= 1e-3){
            continue;
        }
        if(hasNearby(source, cand)){
            particle nextpart(cand.pt(), cand.eta(), cand.phi(),
                              0.0, 0.0, 0.0,
                              std::abs(cand.pdgId()), cand.charge());
            if(doUncertainty){
                addUncertainty(nextpart);
            }
            result.particles.push_back(std::move(nextpart));
            result.nPart++;
            result.sumpt += cand.pt();
        }
    } 
}

class GenMatchProducer : public edm::stream::EDProducer<> {
public:
    explicit GenMatchProducer(const edm::ParameterSet&);
    ~GenMatchProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    int verbose_;
    double dR2thresh_;
    
    double clipval_, cutoff_;
    bool matchCharge_;
    double jetCoreDR2_, softPt_, hardPt_;
    unsigned maxReFit_;

    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;
    edm::InputTag genTag_;
    edm::EDGetTokenT<edm::View<jet>> genToken_;

    edm::InputTag recoPartsTag_;
    edm::EDGetTokenT<edm::View<reco::Candidate>> recoPartsToken_;
    edm::InputTag genPartsTag_;
    edm::EDGetTokenT<edm::View<reco::Candidate>> genPartsToken_;
    bool doLargerCollections_;
};

GenMatchProducer::GenMatchProducer(const edm::ParameterSet& conf)
        : verbose_(conf.getParameter<int>("verbose")),
          dR2thresh_(square(conf.getParameter<double>("dRthresh"))),
          clipval_(conf.getParameter<double>("clipval")),
          cutoff_(conf.getParameter<double>("cutoff")),
          matchCharge_(conf.getParameter<bool>("matchCharge")),
          jetCoreDR2_(square(conf.getParameter<double>("jetCoreThreshold"))),
          softPt_(conf.getParameter<double>("softPt")),
          hardPt_(conf.getParameter<double>("hardPt")),
          maxReFit_(conf.getParameter<unsigned>("maxReFit")),
          recoTag_(conf.getParameter<edm::InputTag>("reco")),
          recoToken_(consumes<edm::View<jet>>(recoTag_)),
          genTag_(conf.getParameter<edm::InputTag>("gen")),
          genToken_(consumes<edm::View<jet>>(genTag_)),
          recoPartsTag_(conf.getParameter<edm::InputTag>("recoParts")),
          recoPartsToken_(consumes<edm::View<reco::Candidate>>(recoPartsTag_)),
          genPartsTag_(conf.getParameter<edm::InputTag>("genParts")),
          genPartsToken_(consumes<edm::View<reco::Candidate>>(genPartsTag_)),
          doLargerCollections_(conf.getParameter<bool>("doLargerCollections")) {
    produces<std::vector<jetmatch>>();
    if(doLargerCollections_){
        produces<std::vector<jetmatch>>("bigReco");
        produces<std::vector<jetmatch>>("bigGen");
    }
}

void GenMatchProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<double>("dRthresh");
  desc.add<double>("clipval");
  desc.add<double>("cutoff");
  desc.add<bool>("matchCharge");
  desc.add<double>("jetCoreThreshold");
  desc.add<double>("softPt");
  desc.add<double>("hardPt");
  desc.add<unsigned>("maxReFit");
  desc.add<edm::InputTag>("reco");
  desc.add<edm::InputTag>("gen");
  desc.add<edm::InputTag>("recoParts");
  desc.add<edm::InputTag>("genParts");
  desc.add<bool>("doLargerCollections");
  desc.add<int>("verbose");
  descriptions.addWithDefaultLabel(desc);
}

void GenMatchProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  if(verbose_){
    printf("Top of GenMatchProducer::produce()\n");
  }
  edm::Handle<edm::View<jet>> reco;
  evt.getByToken(recoToken_, reco);

  edm::Handle<edm::View<jet>> gen;
  evt.getByToken(genToken_, gen);

  edm::Handle<edm::View<reco::Candidate>> genParts;
  edm::Handle<edm::View<reco::Candidate>> recoParts;
  if(doLargerCollections_){
      evt.getByToken(genPartsToken_, genParts);
      evt.getByToken(recoPartsToken_, recoParts);
  }

  auto result = std::make_unique<std::vector<jetmatch>>();
  auto resultBigGen = std::make_unique<std::vector<jetmatch>>();
  auto resultBigReco = std::make_unique<std::vector<jetmatch>>();

  std::vector<bool> taken(gen->size(), false);
  for(unsigned iReco=0; iReco<reco->size(); ++iReco){//for each reco jet
      if(verbose_){
          printf("doing jet %u\n", iReco);
      }
      const jet& jreco = reco->at(iReco);
      int matchedgen = -1;
      for(unsigned iGen=0; iGen<gen->size(); ++iGen){//for each gen jet
          if(taken[iGen]){
              continue;
          }
          const jet& jgen = gen->at(iGen);
    
          double dist = dR2(jreco.eta, jreco.phi, 
                            jgen.eta, jgen.phi);
          if(dist > dR2thresh_){
              continue;
          }

          matchedgen = iGen;
          taken[iGen] = true;
          break;
      }//end for each gen

      if(matchedgen >= 0){//if matched
          if(verbose_){
              printf("matched with %u\n", matchedgen);
          }
          jetmatch next;
          next.iReco = iReco;
          next.iGen = matchedgen;

          const jet& jgen = gen->at(matchedgen);

          if(verbose_){
              printf("fit is between %lu and %lu particles\n", 
                      jreco.particles.size(), jgen.particles.size());
          }
          matcher match (jreco.particles, jgen.particles,
                         clipval_, cutoff_, matchCharge_, 
                         jetCoreDR2_, softPt_, hardPt_,
                         maxReFit_, jreco.eta, jreco.phi);
          match.minimize();
          next.ptrans = match.ptrans();

          result->push_back(std::move(next));
          if(verbose_){
              printf("did fit\n");
          }
      }//end if matched

      if(doLargerCollections_){//if also want to match with everything
          if(verbose_){
              printf("matching with full event\n");
          }
          jet biggen;
          makeNearbyJet(biggen, jreco, genParts, false);

          if(verbose_){
              printf("fit is between %lu and %lu particles\n", 
                      jreco.particles.size(), biggen.particles.size());
          }
          matcher biggenmatch(jreco.particles, biggen.particles, 
                              clipval_, cutoff_, matchCharge_,
                              jetCoreDR2_, softPt_, hardPt_,
                              maxReFit_, jreco.eta, jreco.phi);
          biggenmatch.minimize();

          jetmatch nextbiggen;
          nextbiggen.iReco = iReco;
          nextbiggen.iGen = 999999;
          nextbiggen.ptrans = biggenmatch.ptrans();

          resultBigGen->push_back(std::move(nextbiggen));
          if(verbose_){
              printf("did fit\n");
          }
      }//end if also want to match with everything
  }//end for each reco

  if(doLargerCollections_){//if want to match gen with everything
      for(unsigned iGen=0; iGen<gen->size(); ++iGen){
        if(verbose_){
          printf("matching genJet %u with full event\n", iGen);
        }
        const jet& jgen = gen->at(iGen);

        jet bigreco;
        makeNearbyJet(bigreco, jgen, recoParts, true);

        if(verbose_){
          printf("fit is between %lu and %lu particles\n", 
                  bigreco.particles.size(), jgen.particles.size());
        }
        matcher bigrecomatch(bigreco.particles, jgen.particles,
                             clipval_, cutoff_, matchCharge_,
                             jetCoreDR2_, softPt_, hardPt_,
                             maxReFit_, jgen.eta, jgen.phi);
        bigrecomatch.minimize();

        jetmatch nextbigreco;
        nextbigreco.iReco = 99999999;
        nextbigreco.iGen = iGen;
        nextbigreco.ptrans = bigrecomatch.ptrans();

        resultBigReco->push_back(std::move(nextbigreco));
        if(verbose_){
          printf("did fit\n");
        }
      }
  }

  evt.put(std::move(result));
  if(doLargerCollections_){
      evt.put(std::move(resultBigGen), "bigGen");
      evt.put(std::move(resultBigReco), "bigReco");
  }
}  // end produce()

DEFINE_FWK_MODULE(GenMatchProducer);
