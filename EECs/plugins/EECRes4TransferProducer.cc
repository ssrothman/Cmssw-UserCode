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
#include "SRothman/DataFormats/interface/matching.h"
#include "SRothman/EECs/src/Res4Calculator.h"
#include "SRothman/DataFormats/interface/EEC.h"

#include <memory>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>

class EECRes4TransferProducer : public edm::stream::EDProducer<> {
    explicit EECRes4TransferProducer(const edm::ParameterSet&);
    ~EECRes4TransferProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    EEC::Res4TransferCalculator res4calc_;

    edm::EDGetTokenT<std::vector<simon::jet>> recoJetsToken_;
    edm::EDGetTokenT<std::vector<simon::jet>> genJetsToken_;
    edm::EDGetTokenT<std::vector<matching::jetmatch>> matchesToken_;
};

EECRes4TransferProducer::EECRes4TransferProducer(const edm::ParameterSet& conf) :
        res4calc_(conf.getParameter<edm::ParameterSet>("calculator")),
        recoJetsToken_(consumes<std::vector<simon::jet>>(conf.getParameter<edm::InputTag>("recoJets"))),
        genJetsToken_(consumes<std::vector<simon::jet>>(conf.getParameter<edm::InputTag>("genJets"))),
        matchesToken_(consumes<std::vector<matching::jetmatch>>(conf.getParameter<edm::InputTag>("matches")))
{

    produces<std::vector<EEC::CMSSW_Res4TransferResult>>();
    produces<std::vector<EEC::CMSSW_Res4Result>>();
}

void EECRes4TransferProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    edm::ParameterSetDescription calc_desc;
    EEC::Res4TransferCalculator::fillPSetDescription(calc_desc);
    desc.add<edm::ParameterSetDescription>("calculator", calc_desc);

    desc.add<edm::InputTag>("genJets");
    desc.add<edm::InputTag>("recoJets");
    desc.add<edm::InputTag>("matches");
    descriptions.add("EECRes4TransferProducer", desc);
}

void EECRes4TransferProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
    edm::Handle<std::vector<simon::jet>> recoJets;
    event.getByToken(recoJetsToken_, recoJets);

    edm::Handle<std::vector<simon::jet>> genJets;
    event.getByToken(genJetsToken_, genJets);

    edm::Handle<std::vector<matching::jetmatch>> matches;
    event.getByToken(matchesToken_, matches);

    auto transfer_result = std::make_unique<std::vector<EEC::CMSSW_Res4TransferResult>>();
    auto result = std::make_unique<std::vector<EEC::CMSSW_Res4Result>>();
    result->reserve(matches->size());
    transfer_result->reserve(matches->size());

    for (const auto& match : *matches){
        const auto& genjet = genJets->at(match.iGen);
        const auto& recojet = recoJets->at(match.iReco);
        const auto& tmat = match.tmat;

        transfer_result->emplace_back(match.iReco, match.iGen, res4calc_);
        result->emplace_back(match.iGen, match.iReco, res4calc_);

        res4calc_.compute_precomputed(
                recojet, genjet, tmat, 
                result->back().result, 
                transfer_result->back().result);
    }
    event.put(std::move(result));
    event.put(std::move(transfer_result));
}
