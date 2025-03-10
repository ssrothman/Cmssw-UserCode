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
#include "SRothman/DataFormats/interface/matching.h"

#include <memory>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>

class EECRes4MatchedProducer : public edm::stream::EDProducer<> {
public:
    explicit EECRes4MatchedProducer(const edm::ParameterSet&);
    ~EECRes4MatchedProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    EEC::Res4Calculator res4calc_;

    edm::EDGetTokenT<edm::View<simon::jet>> jetsToken_;
    edm::EDGetTokenT<edm::View<matching::jetmatch>> matchesToken_;

    std::vector<edm::EDGetTokenT<edm::ValueMap<bool>>> flagsTokens_;
};

EECRes4MatchedProducer::EECRes4MatchedProducer(const edm::ParameterSet& conf) :
        res4calc_(conf.getParameter<edm::ParameterSet>("calculator")),
        jetsToken_(consumes<edm::View<simon::jet>>(conf.getParameter<edm::InputTag>("jets"))),
        matchesToken_(consumes<edm::View<matching::jetmatch>>(conf.getParameter<edm::InputTag>("matches")))
{

    std::vector<edm::InputTag> flagsTags = conf.getParameter<std::vector<edm::InputTag>>("flags");
    flagsTokens_.reserve(flagsTags.size());
    for(const auto& tag : flagsTags){
        flagsTokens_.push_back(consumes<edm::ValueMap<bool>>(tag));
    }

    produces<std::vector<EEC::CMSSWRes4Result>>("reco");
    produces<std::vector<EEC::CMSSWRes4Result>>("unmatchedReco");
}

void EECRes4MatchedProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    edm::ParameterSetDescription calc_desc;
    EEC::Res4Calculator::fillPSetDescription(calc_desc);
    desc.add<edm::ParameterSetDescription>("calculator", calc_desc);

    desc.add<edm::InputTag>("jets");
    desc.add<edm::InputTag>("matches");

    desc.add<std::vector<edm::InputTag>>("flags");

    descriptions.add("EECRes4MatchedProducer", desc);
}

void EECRes4MatchedProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
    edm::Handle<edm::View<simon::jet>> jets;
    event.getByToken(jetsToken_, jets);

    edm::Handle<edm::View<matching::jetmatch>> matches;
    event.getByToken(matchesToken_, matches);

    std::vector<bool> passAllFlags(jets->size(), true);
    for(const auto& token : flagsTokens_){
        edm::Handle<edm::ValueMap<bool>> flags;
        event.getByToken(token, flags);

        for(unsigned iJet=0; iJet<jets->size(); ++iJet){
            passAllFlags[iJet] = passAllFlags[iJet] && (*flags)[jets->refAt(iJet)];
        }
    }

    auto result = std::make_unique<std::vector<EEC::CMSSWRes4Result>>();
    auto result_unmatched = std::make_unique<std::vector<EEC::CMSSWRes4Result>>();
    result->reserve(jets->size());
    result_unmatched->reserve(jets->size());

    for (unsigned iJet = 0; iJet < jets->size(); ++iJet){
        if (!passAllFlags[iJet]) {
            continue;
        }

        const auto& jet = jets->at(iJet);

        const matching::jetmatch *thematch = nullptr;
        for(const auto& match : *matches){
            if(match.iReco == iJet){
                thematch = &match;
                break;
            }
        }

        std::vector<bool> matched(jet.nPart, false);
        if(thematch){
            const Eigen::MatrixXd& tmat = thematch->tmat;
            for(unsigned iRecoPart=0; iRecoPart<tmat.rows(); ++iRecoPart){
                for(unsigned iGenPart=0; iGenPart<tmat.cols(); ++iGenPart){
                    if(tmat(iRecoPart, iGenPart) > 0.0){
                        matched[iRecoPart] = true;
                    }
                }
            }
        }

        result->emplace_back(iJet, iJet, res4calc_);
        result_unmatched->emplace_back(iJet, iJet, res4calc_);
        res4calc_.compute_precomputed_matched(
                jet, matched,
                result->back().result,
                result_unmatched->back().result);
    }
    event.put(std::move(result), "reco");
    event.put(std::move(result_unmatched), "unmatchedReco");
}

DEFINE_FWK_MODULE(EECRes4MatchedProducer);
