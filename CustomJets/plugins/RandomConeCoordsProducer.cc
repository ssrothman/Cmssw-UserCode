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

class RandomConeCoordsProducer : public edm::stream::EDProducer<> {
public:
    explicit RandomConeCoordsProducer(const edm::ParameterSet&);
    ~RandomConeCoordsProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    double maxEta_;

    int verbose_;
};

RandomConeCoordsProducer::RandomConeCoordsProducer(const edm::ParameterSet& iConfig) :
    maxEta_(iConfig.getParameter<double>("maxEta")),
    verbose_(iConfig.getParameter<int>("verbose"))
{
    produces<etaPhiCoords>();
}

void RandomConeCoordsProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<double>("maxEta", 3.0);
    desc.add<int>("verbose", 0);
    descriptions.addWithDefaultLabel(desc);
}

void RandomConeCoordsProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
    std::unique_ptr<etaPhiCoords> output(new etaPhiCoords());

    double eta = 2.0 * maxEta_ * (rand() / (double)RAND_MAX) - maxEta_;
    double phi = 2.0 * M_PI * (rand() / (double)RAND_MAX) - M_PI;

    output->eta = eta;
    output->phi = phi;

    iEvent.put(std::move(output));
}

DEFINE_FWK_MODULE(RandomConeCoordsProducer);
