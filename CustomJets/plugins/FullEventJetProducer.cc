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
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"

#include "SRothman/SimonTools/src/jets.h"
#include "SRothman/SimonTools/src/util.h"
#include "SRothman/SimonTools/src/particleThresholds.h"

#include "SRothman/CustomJets/plugins/AddParticle.h"

#include <iostream>
#include <memory>
#include <vector>

class FullEventJetProducer : public edm::stream::EDProducer<> {
public:
    explicit FullEventJetProducer(const edm::ParameterSet&);
    ~FullEventJetProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:

    struct particleThresholds thresholds_;

    bool onlyFromPV_;
    bool onlyCharged_;

    double maxPartEta_;

    unsigned int maxNumPart_, minNumPart_;

    bool applyPuppi_;

    edm::InputTag partSrc_;
    edm::EDGetTokenT<edm::View<reco::Candidate>> partToken_;

    edm::InputTag evtSelSrc_;
    edm::EDGetTokenT<bool> evtSelToken_;
    bool doEvtSel_;

    int verbose_;
};

FullEventJetProducer::FullEventJetProducer(const edm::ParameterSet& conf)
        : thresholds_(conf.getParameter<edm::ParameterSet>("particleThresholds")),
          onlyFromPV_(conf.getParameter<bool>("onlyFromPV")),
          onlyCharged_(conf.getParameter<bool>("onlyCharged")),
          maxPartEta_(conf.getParameter<double>("maxPartEta")),
          maxNumPart_(conf.getParameter<unsigned>("maxNumPart")),
          applyPuppi_(conf.getParameter<bool>("applyPuppi")),
          partSrc_(conf.getParameter<edm::InputTag>("partSrc")),
          partToken_(consumes<edm::View<reco::Candidate>>(partSrc_)),
          evtSelSrc_(conf.getParameter<edm::InputTag>("eventSelection")),
          evtSelToken_(consumes<bool>(evtSelSrc_)),
          doEvtSel_(conf.getParameter<bool>("doEventSelection")),
          verbose_(conf.getParameter<int>("verbose")){
    produces<std::vector<jet>>();
}

void FullEventJetProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
  
     //desc.add<edm::ParameterSetDescription>(
      //      "particleThresholds", 
        //    particleThresholds::makePSetDescription());

    desc.add<bool>("onlyFromPV");
    desc.add<bool>("onlyCharged");

    desc.add<double>("maxPartEta");

    desc.add<unsigned>("maxNumPart");

    desc.add<bool>("applyPuppi");

    desc.add<edm::InputTag>("eventSelection");
    desc.add<bool>("doEventSelection");

    desc.add<edm::InputTag>("partSrc");

    desc.add<int>("verbose");

    descriptions.addWithDefaultLabel(desc);
}

void FullEventJetProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of FullEventJetProducer::produce()\n");
    }
    edm::Handle<edm::View<reco::Candidate>> parts;
    evt.getByToken(partToken_, parts);

    auto result = std::make_unique<std::vector<jet>>();

    if(doEvtSel_){
        edm::Handle<bool> evtSel;
        evt.getByToken(evtSelToken_, evtSel);
        if(!*evtSel){
            evt.put(std::move(result));
            return;
        }
        if(verbose_){
            printf("passed event selection\n");
        }
    }

    jet ans;
    ans.pt = -9999;
    ans.eta = 9999;
    ans.phi = 9999;
    ans.iJet = 0;
    ans.jecfactor = 9999;

    ans.nPart = 0;
    ans.sumpt = 0.0;
    ans.particles.clear();

    for(size_t iPart=0; iPart < parts->size(); ++iPart){
        const auto& part = parts->at(iPart);

        const auto* partptr=dynamic_cast<const pat::PackedCandidate*>(&part);
        const auto* genptr=dynamic_cast<const pat::PackedGenParticle*>(&part);
        if(partptr){
            addParticle(partptr, ans, 1.0, 
                    applyPuppi_, false, 
                    onlyFromPV_, onlyCharged_,
                    maxPartEta_, thresholds_,
                    maxNumPart_);
        } else if(genptr){
            addParticle(genptr, ans, 1.0,
                    applyPuppi_, false,
                    onlyFromPV_, onlyCharged_,
                    maxPartEta_, thresholds_,
                    maxNumPart_);
        } else {
            throw cms::Exception("FullEventJetProducer::produce()")
                << "particle is neither pat::PackedCandidate nor pat::PackedGenParticle\n";
        }
    }

    std::sort(ans.particles.begin(), ans.particles.end(), [](const particle& a, const particle& b){
        return a.pt > b.pt;
    });

    if(verbose_){
        printf("Made fullevent jet with %lu particles\n",ans.particles.size());
    }

    if(ans.nPart >= minNumPart_){
        result->push_back(std::move(ans));
        if(verbose_){
            printf("pushed back\n");
        }
    }

    evt.put(std::move(result));
    if(verbose_){
        printf("put into event\n");
    }
}  // end produce()

DEFINE_FWK_MODULE(FullEventJetProducer);
