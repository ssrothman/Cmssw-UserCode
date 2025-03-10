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

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"

#include "SRothman/SimonTools/src/jet.h"
#include "SRothman/EECs/src/Res4Calculator.h"
#include "SRothman/DataFormats/interface/EEC.h"

#include <memory>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>

class EECRes4Producer : public edm::stream::EDProducer<> {
public:
    explicit EECRes4Producer(const edm::ParameterSet&);
    ~EECRes4Producer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    EEC::Res4Calculator res4calc_;

    edm::EDGetTokenT<edm::View<simon::jet>> jetsToken_;

    std::vector<edm::EDGetTokenT<edm::ValueMap<bool>>> flagsTokens_;
};

EECRes4Producer::EECRes4Producer(const edm::ParameterSet& conf) :
        res4calc_(conf.getParameter<edm::ParameterSet>("calculator")),
        jetsToken_(consumes<edm::View<simon::jet>>(conf.getParameter<edm::InputTag>("jets"))) {

    std::vector<edm::InputTag> flagsTags = conf.getParameter<std::vector<edm::InputTag>>("flags");
    flagsTokens_.reserve(flagsTags.size());
    for(const auto& tag : flagsTags){
        flagsTokens_.push_back(consumes<edm::ValueMap<bool>>(tag));
    }

    produces<std::vector<EEC::CMSSWRes4Result>>("reco");
}

void EECRes4Producer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    edm::ParameterSetDescription calc_desc;
    EEC::Res4Calculator::fillPSetDescription(calc_desc);
    desc.add<edm::ParameterSetDescription>("calculator", calc_desc);

    desc.add<edm::InputTag>("jets");

    desc.add<std::vector<edm::InputTag>>("flags");

    descriptions.add("EECRes4Producer", desc);
}

void EECRes4Producer::produce(edm::Event& event, const edm::EventSetup& setup) {
    edm::Handle<edm::View<simon::jet>> jets;
    event.getByToken(jetsToken_, jets);

    std::vector<bool> passAllFlags(jets->size(), true);
    for(const auto& token : flagsTokens_){
        edm::Handle<edm::ValueMap<bool>> flags;
        event.getByToken(token, flags);

        for(size_t i = 0; i < jets->size(); ++i){
            passAllFlags[i] = passAllFlags[i] && (*flags)[jets->refAt(i)];
        }
    }

    auto result = std::make_unique<std::vector<EEC::CMSSWRes4Result>>();
    result->reserve(jets->size());

    for (unsigned iJet = 0; iJet < jets->size(); ++iJet){
        if (!passAllFlags[iJet]) continue;

        const auto& jet = jets->at(iJet);
        result->emplace_back(iJet, iJet, res4calc_);
        res4calc_.compute_precomputed(jet, result->back().result);
    }
    event.put(std::move(result), "reco");
}

DEFINE_FWK_MODULE(EECRes4Producer);
