#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "SRothman/SimonTools/src/jet.h"

#include "SRothman/Matching/src/v2/TrackMatcher.h"

#include "SRothman/DataFormats/interface/matching.h"

#include <memory>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>

class TrackMatchProducer : public edm::stream::EDProducer<> {
public:
    explicit TrackMatchProducer(const edm::ParameterSet&);
    ~TrackMatchProducer() override = default;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    matching::TrackMatcher matcher_;

    edm::EDGetTokenT<std::vector<simon::jet>> recoJetsToken_;
    edm::EDGetTokenT<std::vector<simon::jet>> genJetsToken_;
};

TrackMatchProducer::TrackMatchProducer(const edm::ParameterSet& config) :
        matcher_(config.getParameter<edm::ParameterSet>("matcher")),
        recoJetsToken_(consumes<std::vector<simon::jet>>(config.getParameter<edm::InputTag>("recoJets"))),
        genJetsToken_(consumes<std::vector<simon::jet>>(config.getParameter<edm::InputTag>("genJets")))
{

    produces<std::vector<matching::jetmatch>>();
}

void TrackMatchProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
    edm::Handle<std::vector<simon::jet>> recojets;
    event.getByToken(recoJetsToken_, recojets);

    edm::Handle<std::vector<simon::jet>> genjets;
    event.getByToken(genJetsToken_, genjets);

    auto result = std::make_unique<std::vector<matching::jetmatch>>();

    matching::matchvec matches;
    matcher_.matchJets(*genjets, *recojets, matches);

    for(const auto& jetpair : matches){
        matching::jetmatch match;
        match.iReco = jetpair.iReco;
        match.iGen = jetpair.iGen;

        matcher_.matchParticles(genjets->at(match.iGen), recojets->at(match.iReco), match.tmat);

        result->push_back(match);
    }

    event.put(std::move(result));
}

void TrackMatchProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    edm::ParameterSetDescription matcherDesc;
    matching::TrackMatcher::fillPSetDescription(matcherDesc);
    desc.add<edm::ParameterSetDescription>("matcher", matcherDesc);

    desc.add<edm::InputTag>("recoJets");
    desc.add<edm::InputTag>("genJets");

    descriptions.addWithDefaultLabel(desc);
}

DEFINE_FWK_MODULE(TrackMatchProducer);
