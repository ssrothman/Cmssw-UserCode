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

#include "SRothman/SimonTools/src/jet.h"
#include "SRothman/SimonTools/src/util.h"
#include "SRothman/SimonTools/src/particleSelector.h"
#include "SRothman/SimonTools/src/computeJetMass.h"

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
    simon::particleSelector selector_;

    edm::InputTag partSrc_;
    edm::EDGetTokenT<edm::View<reco::Candidate>> partToken_;

    int verbose_;
};

FullEventJetProducer::FullEventJetProducer(const edm::ParameterSet& conf)
        : 
          selector_(conf.getParameter<edm::ParameterSet>("selector")),
          partSrc_(conf.getParameter<edm::InputTag>("partSrc")),
          partToken_(consumes<edm::View<reco::Candidate>>(partSrc_)),
          verbose_(conf.getParameter<int>("verbose")){
    produces<std::vector<simon::jet>>();
}

void FullEventJetProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

  edm::ParameterSetDescription selectorDesc;
  simon::particleSelector::fillPSetDescription(selectorDesc);
  desc.add<edm::ParameterSetDescription>("selector", selectorDesc);

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

    auto result = std::make_unique<std::vector<simon::jet>>();

    simon::jet ans;
    ans.pt = -9999;
    ans.eta = 9999;
    ans.phi = 9999;
    ans.iJet = 0;
    ans.jecfactor = 9999;

    ans.nPart = 0;
    ans.sumpt = 0.0;
    ans.particles.clear();

    selector_.buildJet(parts->ptrs(), ans);

    if(verbose_){
        printf("Made fullevent jet with %lu particles\n",ans.particles.size());
    }

    simon::computeJetMass(ans);

    result->push_back(std::move(ans));
    if(verbose_){
        printf("pushed back\n");
    }

    evt.put(std::move(result));
    if(verbose_){
        printf("put into event\n");
    }
}  // end produce()

DEFINE_FWK_MODULE(FullEventJetProducer);
