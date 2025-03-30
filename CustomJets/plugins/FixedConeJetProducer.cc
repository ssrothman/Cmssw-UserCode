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
#include "DataFormats/Math/interface/deltaR.h"

#include "SRothman/SimonTools/src/jet.h"
#include "SRothman/SimonTools/src/util.h"
#include "SRothman/SimonTools/src/etaPhiCoords.h"

#include "SRothman/SimonTools/src/particleSelector.h"
#include "SRothman/SimonTools/src/computeJetMass.h"

#include <iostream>
#include <memory>
#include <vector>

class FixedConeJetProducer : public edm::stream::EDProducer<> {
public:
    explicit FixedConeJetProducer(const edm::ParameterSet&);
    ~FixedConeJetProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    simon::particleSelector selector_;

    edm::InputTag coordSrc_;
    edm::EDGetTokenT<simon::etaPhiCoords> coordToken_;

    edm::InputTag partSrc_;
    edm::EDGetTokenT<edm::View<reco::Candidate>> partToken_;

    double conesize_;

    int verbose_;
};

FixedConeJetProducer::FixedConeJetProducer(const edm::ParameterSet& conf)
        : 
          selector_(conf.getParameter<edm::ParameterSet>("selector")),
          coordSrc_(conf.getParameter<edm::InputTag>("coords")),
          coordToken_(consumes<simon::etaPhiCoords>(coordSrc_)),
          partSrc_(conf.getParameter<edm::InputTag>("particles")),
          partToken_(consumes<edm::View<reco::Candidate>>(partSrc_)),
          conesize_(conf.getParameter<double>("conesize")),
          verbose_(conf.getParameter<int>("verbose")){
    produces<std::vector<simon::jet>>();
}

void FixedConeJetProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  edm::ParameterSetDescription selectorDesc;
  simon::particleSelector::fillPSetDescription(selectorDesc);
  desc.add<edm::ParameterSetDescription>("selector", selectorDesc);

  desc.add<edm::InputTag>("coords");

  desc.add<edm::InputTag>("particles");

  desc.add<int>("verbose");

  desc.add<double>("conesize");

  descriptions.addWithDefaultLabel(desc);
}

void FixedConeJetProducer::produce(edm::Event& evt, 
                                   const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of FixedConeJetProducer::produce()\n");
    }

    edm::Handle<simon::etaPhiCoords> coords;
    evt.getByToken(coordToken_, coords);

    if(verbose_){
        printf("got coords\n");
    }

    edm::Handle<edm::View<reco::Candidate>> parts;
    evt.getByToken(partToken_, parts);

    if(verbose_){
        printf("got particles\n");
    }

    auto result = std::make_unique<std::vector<simon::jet>>();

    simon::jet ans;
    ans.eta = coords->eta;
    ans.phi = coords->phi;
    ans.iJet = 9999;
    ans.jecfactor = 9999;
    ans.pt = 0;

    auto thefilter = [coords, &conesize_=conesize_](const edm::Ptr<reco::Candidate>& part){
        return reco::deltaR(part->eta(), part->phi(),
                            coords->eta, coords->phi) < conesize_;
    };

    selector_.buildJet(parts->ptrs(), ans, &thefilter);

    ans.pt = ans.rawpt;
    computeJetMass(ans);

    result->push_back(std::move(ans));

    if(verbose_){
        printf("pushed back\n");
    }

    evt.put(std::move(result));
    if(verbose_){
        printf("put into event\n");
    }
}  // end produce()

DEFINE_FWK_MODULE(FixedConeJetProducer);
