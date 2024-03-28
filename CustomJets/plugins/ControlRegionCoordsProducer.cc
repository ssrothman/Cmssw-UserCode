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

#include "SRothman/SimonTools/src/jets.h"
#include "SRothman/SimonTools/src/util.h"
#include "SRothman/SimonTools/src/etaPhiCoords.h"

#include <iostream>
#include <memory>
#include <vector>

class ControlRegionCoordsProducer : public edm::stream::EDProducer<> {
public:
    explicit ControlRegionCoordsProducer(const edm::ParameterSet&);
    ~ControlRegionCoordsProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    edm::InputTag muonSrc_;
    edm::EDGetTokenT<edm::View<pat::Muon>> muonToken_;

    double maxEta_;

    int verbose_;
};

ControlRegionCoordsProducer::ControlRegionCoordsProducer(const edm::ParameterSet& iConfig) :
    muonSrc_(iConfig.getParameter<edm::InputTag>("muonSrc")),
    muonToken_(consumes<edm::View<pat::Muon>>(muonSrc_)),
    maxEta_(iConfig.getParameter<double>("maxEta")),
    verbose_(iConfig.getParameter<int>("verbose"))
{
    produces<etaPhiCoords>();
}

void ControlRegionCoordsProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<double>("maxEta", 3.0);
    desc.add<int>("verbose", 0);
    desc.add<edm::InputTag>("muonSrc", edm::InputTag("slimmedMuons"));
    descriptions.addWithDefaultLabel(desc);
}

void ControlRegionCoordsProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
    edm::Handle<edm::View<pat::Muon>> muons;
    iEvent.getByToken(muonToken_, muons);

    std::unique_ptr<etaPhiCoords> output(new etaPhiCoords());

    double eta, phi;

    if(muons->size() < 2){
        eta = 2.0 * maxEta_ * (rand() / (double)RAND_MAX) - maxEta_;
        phi = 2.0 * M_PI * (rand() / (double)RAND_MAX) - M_PI;
    } else {
        const auto& mu1 = muons->at(0);
        const auto& mu2 = muons->at(1);
        const auto& z = mu1.p4() + mu2.p4();

        eta = -z.eta();
        phi = z.phi() + M_PI/2;
        if(phi > M_PI) phi -= 2.0 * M_PI;
    }

    output->eta = eta;
    output->phi = phi;

    if(output->eta > maxEta_){
        output->eta = maxEta_;
    } else if(output->eta < -maxEta_){
        output->eta = -maxEta_;
    }

    iEvent.put(std::move(output));
}

DEFINE_FWK_MODULE(ControlRegionCoordsProducer);
