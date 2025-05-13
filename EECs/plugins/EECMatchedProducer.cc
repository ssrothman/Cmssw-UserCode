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
#include "SRothman/DataFormats/interface/EEC.h"
#include "SRothman/DataFormats/interface/matching.h"

#include <memory>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>

template <class Calculator, class ResultType>
class EECMatchedProducer : public edm::stream::EDProducer<> {
public:
    explicit EECMatchedProducer(const edm::ParameterSet&);
    ~EECMatchedProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    Calculator calc_;

    edm::EDGetTokenT<edm::View<simon::jet>> jetsToken_;
    edm::EDGetTokenT<edm::View<matching::jetmatch>> matchesToken_;

    std::vector<edm::EDGetTokenT<edm::ValueMap<bool>>> flagsTokens_;
};

template <class Calculator, class ResultType>
EECMatchedProducer<Calculator, ResultType>::EECMatchedProducer(const edm::ParameterSet& conf) :
        calc_(conf.getParameter<edm::ParameterSet>("calculator")),
        jetsToken_(consumes<edm::View<simon::jet>>(conf.getParameter<edm::InputTag>("jets"))),
        matchesToken_(consumes<edm::View<matching::jetmatch>>(conf.getParameter<edm::InputTag>("matches")))
{

    std::vector<edm::InputTag> flagsTags = conf.getParameter<std::vector<edm::InputTag>>("flags");
    flagsTokens_.reserve(flagsTags.size());
    for(const auto& tag : flagsTags){
        flagsTokens_.push_back(consumes<edm::ValueMap<bool>>(tag));
    }

    produces<std::vector<EEC::CMSSWResult<ResultType>>>("reco");
    produces<std::vector<EEC::CMSSWResult<ResultType>>>("unmatchedReco");
}

template <class Calculator, class ResultType>
void EECMatchedProducer<Calculator, ResultType>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    edm::ParameterSetDescription calc_desc;
    Calculator::fillPSetDescription(calc_desc);
    desc.add<edm::ParameterSetDescription>("calculator", calc_desc);

    desc.add<edm::InputTag>("jets");
    desc.add<edm::InputTag>("matches");

    desc.add<std::vector<edm::InputTag>>("flags");

    descriptions.addWithDefaultLabel(desc);
}

template <class Calculator, class ResultType>
void EECMatchedProducer<Calculator, ResultType>::produce(edm::Event& event, const edm::EventSetup& setup) {
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

    auto result = std::make_unique<std::vector<EEC::CMSSWResult<ResultType>>>();
    auto result_unmatched = std::make_unique<std::vector<EEC::CMSSWResult<ResultType>>>();
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

        result->emplace_back(iJet, iJet, calc_);
        result_unmatched->emplace_back(iJet, iJet, calc_);
        calc_.compute_precomputed_matched(
                jet, matched,
                result->back().result,
                result_unmatched->back().result);
    }
    event.put(std::move(result), "reco");
    event.put(std::move(result_unmatched), "unmatchedReco");
}

#include "SRothman/EECs/src/Res4Calculator.h"
#include "SRothman/EECs/src/Res4Result.h"

typedef EECMatchedProducer<EEC::Res4Calculator, 
                              EEC::Res4Result_MultiArray> EECRes4MatchedArrayProducer;
typedef EECMatchedProducer<EEC::Res4Calculator, 
                              EEC::Res4Result_Vector> EECRes4MatchedVectorProducer;
typedef EECMatchedProducer<EEC::Res4Calculator, 
                              EEC::Res4Result_Unbinned> EECRes4MatchedUnbinnedProducer;

DEFINE_FWK_MODULE(EECRes4MatchedVectorProducer);
DEFINE_FWK_MODULE(EECRes4MatchedUnbinnedProducer);
DEFINE_FWK_MODULE(EECRes4MatchedArrayProducer);

#include "SRothman/EECs/src/Res3Calculator.h"
#include "SRothman/EECs/src/Res3Result.h"

typedef EECMatchedProducer<EEC::Res3Calculator, 
                              EEC::Res3Result_MultiArray> EECRes3MatchedArrayProducer;
typedef EECMatchedProducer<EEC::Res3Calculator, 
                              EEC::Res3Result_Vector> EECRes3MatchedVectorProducer;
typedef EECMatchedProducer<EEC::Res3Calculator, 
                              EEC::Res3Result_Unbinned> EECRes3MatchedUnbinnedProducer;

DEFINE_FWK_MODULE(EECRes3MatchedVectorProducer);
DEFINE_FWK_MODULE(EECRes3MatchedUnbinnedProducer);
DEFINE_FWK_MODULE(EECRes3MatchedArrayProducer);

#include "SRothman/EECs/src/ProjCalculator.h"
#include "SRothman/EECs/src/ProjResult.h"

typedef EECMatchedProducer<EEC::ProjCalculator, 
                              EEC::ProjResult_Array> EECProjMatchedArrayProducer;
typedef EECMatchedProducer<EEC::ProjCalculator, 
                              EEC::ProjResult_Vector> EECProjMatchedVectorProducer;
typedef EECMatchedProducer<EEC::ProjCalculator, 
                              EEC::ProjResult_Unbinned> EECProjMatchedUnbinnedProducer;

DEFINE_FWK_MODULE(EECProjMatchedVectorProducer);
DEFINE_FWK_MODULE(EECProjMatchedUnbinnedProducer);
DEFINE_FWK_MODULE(EECProjMatchedArrayProducer);

