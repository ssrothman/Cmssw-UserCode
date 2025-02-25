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
public:
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

    produces<std::vector<EEC::CMSSWRes4TransferResult>>("transfer");
    produces<std::vector<EEC::CMSSWRes4Result>>("gen");
    produces<std::vector<EEC::CMSSWRes4Result>>("unmatchedGen");
    produces<std::vector<EEC::CMSSWRes4Result>>("untransferedGen");
    produces<std::vector<EEC::CMSSWRes4Result>>("untransferedReco");
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

    auto transfer_result = std::make_unique<std::vector<EEC::CMSSWRes4TransferResult>>();
    auto gen_result = std::make_unique<std::vector<EEC::CMSSWRes4Result>>();
    auto unmatched_gen_result = std::make_unique<std::vector<EEC::CMSSWRes4Result>>();
    auto untransfered_gen_result = std::make_unique<std::vector<EEC::CMSSWRes4Result>>();
    auto untransfered_reco_result = std::make_unique<std::vector<EEC::CMSSWRes4Result>>();

    gen_result->reserve(matches->size());
    unmatched_gen_result->reserve(matches->size());
    transfer_result->reserve(matches->size());
    untransfered_gen_result->reserve(matches->size());
    untransfered_reco_result->reserve(matches->size());

    for (const auto& match : *matches){
        const auto& genjet = genJets->at(match.iGen);
        const auto& recojet = recoJets->at(match.iReco);
        const auto& tmat = match.tmat;

        transfer_result->emplace_back(match.iReco, match.iGen, res4calc_);
        gen_result->emplace_back(match.iGen, match.iReco, res4calc_.get_axes_gen());
        unmatched_gen_result->emplace_back(match.iGen, match.iReco, res4calc_.get_axes_gen());
        untransfered_reco_result->emplace_back(match.iReco, match.iReco, res4calc_.get_axes_reco());
        untransfered_gen_result->emplace_back(match.iGen, match.iReco, res4calc_.get_axes_gen());

        res4calc_.compute_precomputed(
                recojet, genjet, tmat, 
                gen_result->back().result, 
                unmatched_gen_result->back().result,
                transfer_result->back().result,
                untransfered_reco_result->back().result,
                untransfered_gen_result->back().result);
    }
    event.put(std::move(gen_result), "gen");
    event.put(std::move(unmatched_gen_result), "unmatchedGen");
    event.put(std::move(transfer_result), "transfer");
    event.put(std::move(untransfered_reco_result), "untransferedReco");
    event.put(std::move(untransfered_gen_result), "untransferedGen");
}

DEFINE_FWK_MODULE(EECRes4TransferProducer);
