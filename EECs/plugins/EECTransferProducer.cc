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
#include "SRothman/DataFormats/interface/matching.h"
#include "SRothman/DataFormats/interface/EEC.h"

#include <memory>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>

template <class Calculator, class ResultType, class TransferResultType, class BasicCalculator>
class EECTransferProducer : public edm::stream::EDProducer<> {
public:
    explicit EECTransferProducer(const edm::ParameterSet&);
    ~EECTransferProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    bool genIsMatched(const unsigned iGen, const edm::View<matching::jetmatch>& matches) const;

    Calculator calc_;
    BasicCalculator basicCalc_;

    edm::EDGetTokenT<edm::View<simon::jet>> recoJetsToken_;
    edm::EDGetTokenT<edm::View<simon::jet>> genJetsToken_;
    edm::EDGetTokenT<edm::View<matching::jetmatch>> matchesToken_;

    std::vector<edm::EDGetTokenT<edm::ValueMap<bool>>> recoFlagsTokens_;
    std::vector<edm::EDGetTokenT<edm::ValueMap<bool>>> genFlagsTokens_;
};

template <class Calculator, class ResultType, class TransferResultType, class BasicCalculator>
bool EECTransferProducer<Calculator, ResultType, TransferResultType, BasicCalculator>::genIsMatched(const unsigned iGen, const edm::View<matching::jetmatch>& matches) const {
    for(const auto& match : matches){
        if(match.iGen==iGen) return true;
    }
    return false;
}

template <class Calculator, class ResultType, class TransferResultType, class BasicCalculator>
EECTransferProducer<Calculator, ResultType, TransferResultType, BasicCalculator>::EECTransferProducer(const edm::ParameterSet& conf) :
        calc_(conf.getParameter<edm::ParameterSet>("calculator")),
        basicCalc_(conf.getParameter<edm::ParameterSet>("basicCalculator")),
        recoJetsToken_(consumes<edm::View<simon::jet>>(conf.getParameter<edm::InputTag>("recoJets"))),
        genJetsToken_(consumes<edm::View<simon::jet>>(conf.getParameter<edm::InputTag>("genJets"))),
        matchesToken_(consumes<edm::View<matching::jetmatch>>(conf.getParameter<edm::InputTag>("matches")))
{

    std::vector<edm::InputTag> recoFlagsTags = conf.getParameter<std::vector<edm::InputTag>>("recoFlags");
    recoFlagsTokens_.reserve(recoFlagsTags.size());
    for(const auto& tag : recoFlagsTags){
        recoFlagsTokens_.push_back(consumes<edm::ValueMap<bool>>(tag));
    }

    std::vector<edm::InputTag> genFlagsTags = conf.getParameter<std::vector<edm::InputTag>>("genFlags");
    genFlagsTokens_.reserve(genFlagsTags.size());
    for(const auto& tag : genFlagsTags){
        genFlagsTokens_.push_back(consumes<edm::ValueMap<bool>>(tag));
    }

    produces<std::vector<EEC::CMSSWTransferResult<TransferResultType>>>("transfer");
    produces<std::vector<EEC::CMSSWResult<ResultType>>>("gen");
    produces<std::vector<EEC::CMSSWResult<ResultType>>>("unmatchedGen");
    produces<std::vector<EEC::CMSSWResult<ResultType>>>("untransferedGen");
    produces<std::vector<EEC::CMSSWResult<ResultType>>>("untransferedReco");
}

template <class Calculator, class ResultType, class TransferResultType, class BasicCalculator>
void EECTransferProducer<Calculator, ResultType, TransferResultType, BasicCalculator>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    edm::ParameterSetDescription calc_desc;
    Calculator::fillPSetDescription(calc_desc);
    desc.add<edm::ParameterSetDescription>("calculator", calc_desc);

    edm::ParameterSetDescription basic_calc_desc;
    BasicCalculator::fillPSetDescription(basic_calc_desc);
    desc.add<edm::ParameterSetDescription>("basicCalculator", basic_calc_desc);

    desc.add<edm::InputTag>("genJets");
    desc.add<edm::InputTag>("recoJets");
    desc.add<edm::InputTag>("matches");

    desc.add<std::vector<edm::InputTag>>("recoFlags");
    desc.add<std::vector<edm::InputTag>>("genFlags");

    descriptions.addWithDefaultLabel(desc);
}

template <class Calculator, class ResultType, class TransferResultType, class BasicCalculator>
void EECTransferProducer<Calculator, ResultType, TransferResultType, BasicCalculator>::produce(edm::Event& event, const edm::EventSetup& setup) {
    edm::Handle<edm::View<simon::jet>> recoJets;
    event.getByToken(recoJetsToken_, recoJets);

    edm::Handle<edm::View<simon::jet>> genJets;
    event.getByToken(genJetsToken_, genJets);

    edm::Handle<edm::View<matching::jetmatch>> matches;
    event.getByToken(matchesToken_, matches);

    std::vector<bool> passAllFlagsRECO(recoJets->size(), true);
    for(const auto& token : recoFlagsTokens_){
        edm::Handle<edm::ValueMap<bool>> flags;
        event.getByToken(token, flags);

        for(unsigned iJet=0; iJet<recoJets->size(); ++iJet){
            passAllFlagsRECO[iJet] = passAllFlagsRECO[iJet] && (*flags)[recoJets->refAt(iJet)];
        }
    }

    std::vector<bool> passAllFlagsGEN(genJets->size(), true);
    for(const auto& token : genFlagsTokens_){
        edm::Handle<edm::ValueMap<bool>> flags;
        event.getByToken(token, flags);
        for(unsigned iJet=0; iJet<genJets->size(); ++iJet){
            passAllFlagsGEN[iJet] = passAllFlagsGEN[iJet] && (*flags)[genJets->refAt(iJet)];
        }
    }

    auto transfer_result = std::make_unique<std::vector<EEC::CMSSWTransferResult<TransferResultType>>>();
    auto gen_result = std::make_unique<std::vector<EEC::CMSSWResult<ResultType>>>();
    auto unmatched_gen_result = std::make_unique<std::vector<EEC::CMSSWResult<ResultType>>>();

    //may not be used depending on Calculator::HAS_UNTRANSFERED
    [[maybe_unused]] auto untransfered_gen_result = std::make_unique<std::vector<EEC::CMSSWResult<ResultType>>>();
    [[maybe_unused]] auto untransfered_reco_result = std::make_unique<std::vector<EEC::CMSSWResult<ResultType>>>();

    gen_result->reserve(matches->size());
    unmatched_gen_result->reserve(matches->size());
    transfer_result->reserve(matches->size());

    if constexpr (Calculator::HAS_UNTRANSFERED){
        untransfered_gen_result->reserve(matches->size());
        untransfered_reco_result->reserve(matches->size());
    }

    for (const auto& match : *matches){
        if(!passAllFlagsRECO[match.iReco] && !passAllFlagsGEN[match.iGen]) continue;

        const auto& genjet = genJets->at(match.iGen);
        const auto& recojet = recoJets->at(match.iReco);
        const auto& tmat = match.tmat;

        transfer_result->emplace_back(match.iReco, match.iGen, calc_);
        gen_result->emplace_back(match.iGen, match.iReco, calc_.get_axes_gen());
        unmatched_gen_result->emplace_back(match.iGen, match.iReco, calc_.get_axes_gen());

        if constexpr(Calculator::HAS_UNTRANSFERED){
            untransfered_reco_result->emplace_back(match.iReco, match.iReco, calc_.get_axes_reco());
            untransfered_gen_result->emplace_back(match.iGen, match.iReco, calc_.get_axes_gen());

            calc_.compute_precomputed(
                    recojet, genjet, tmat, 
                    gen_result->back().result, 
                    unmatched_gen_result->back().result,
                    transfer_result->back().result,
                    untransfered_reco_result->back().result,
                    untransfered_gen_result->back().result);
        } else {
            calc_.compute_precomputed(
                    recojet, genjet, tmat, 
                    gen_result->back().result, 
                    unmatched_gen_result->back().result,
                    transfer_result->back().result);
        }
    }

    for(unsigned iGen=0; iGen<genJets->size(); ++iGen){
        if (!passAllFlagsGEN[iGen] || genIsMatched(iGen, *matches)) continue;
        
        const auto& genjet = genJets->at(iGen);

        gen_result->emplace_back(iGen, -1, calc_.get_axes_gen());

        basicCalc_.compute_precomputed(
            genjet, 
            gen_result->back().result
        );

        if constexpr(Calculator::HAS_UNTRANSFERED){
            untransfered_gen_result->emplace_back(iGen, -1, calc_.get_axes_gen()); // untransfered should be empty
            untransfered_gen_result->back().result.set_pt_denom(gen_result->back().result.get_pt_denom()); // but we need to set the pt_denom for untransfered
        }

        unmatched_gen_result->push_back(gen_result->back()); // unmatched is the same as gen for unmatched gen jets
    }

    event.put(std::move(gen_result), "gen");
    event.put(std::move(unmatched_gen_result), "unmatchedGen");
    event.put(std::move(transfer_result), "transfer");
    if constexpr (Calculator::HAS_UNTRANSFERED){
        event.put(std::move(untransfered_reco_result), "untransferedReco");
        event.put(std::move(untransfered_gen_result), "untransferedGen");
    }
}

#include "SRothman/EECs/src/Res4Calculator.h"
#include "SRothman/EECs/src/Res4Result.h"
#include "SRothman/EECs/src/Res4TransferResult.h"

typedef EECTransferProducer<EEC::Res4TransferCalculator, 
                               EEC::Res4Result_Vector, 
                               EEC::Res4TransferResult_Vector,
                               EEC::Res4Calculator> EECRes4TransferVectorProducer;
typedef EECTransferProducer<EEC::Res4TransferCalculator, 
                               EEC::Res4Result_Unbinned, 
                               EEC::Res4TransferResult_Unbinned,
                               EEC::Res4Calculator> EECRes4TransferUnbinnedProducer;
typedef EECTransferProducer<EEC::Res4TransferCalculator, 
                               EEC::Res4Result_MultiArray, 
                               EEC::Res4TransferResult_MultiArray,
                               EEC::Res4Calculator> EECRes4TransferArrayProducer;

DEFINE_FWK_MODULE(EECRes4TransferVectorProducer);
DEFINE_FWK_MODULE(EECRes4TransferUnbinnedProducer);
DEFINE_FWK_MODULE(EECRes4TransferArrayProducer);

#include "SRothman/EECs/src/Res3Calculator.h"
#include "SRothman/EECs/src/Res3Result.h"
#include "SRothman/EECs/src/Res3TransferResult.h"

typedef EECTransferProducer<EEC::Res3TransferCalculator, 
                               EEC::Res3Result_Vector, 
                               EEC::Res3TransferResult_Vector,
                               EEC::Res3Calculator> EECRes3TransferVectorProducer;
typedef EECTransferProducer<EEC::Res3TransferCalculator, 
                               EEC::Res3Result_Unbinned, 
                               EEC::Res3TransferResult_Unbinned,
                               EEC::Res3Calculator> EECRes3TransferUnbinnedProducer;
typedef EECTransferProducer<EEC::Res3TransferCalculator, 
                               EEC::Res3Result_MultiArray, 
                               EEC::Res3TransferResult_MultiArray,
                               EEC::Res3Calculator> EECRes3TransferArrayProducer;

DEFINE_FWK_MODULE(EECRes3TransferVectorProducer);
DEFINE_FWK_MODULE(EECRes3TransferUnbinnedProducer);
DEFINE_FWK_MODULE(EECRes3TransferArrayProducer);

#include "SRothman/EECs/src/ProjCalculator.h"
#include "SRothman/EECs/src/ProjResult.h"
#include "SRothman/EECs/src/ProjTransferResult.h"

typedef EECTransferProducer<EEC::ProjTransferCalculator, 
                               EEC::ProjResult_Vector, 
                               EEC::ProjTransferResult_Vector,
                               EEC::ProjCalculator> EECProjTransferVectorProducer;
typedef EECTransferProducer<EEC::ProjTransferCalculator, 
                               EEC::ProjResult_Unbinned, 
                               EEC::ProjTransferResult_Unbinned,
                               EEC::ProjCalculator> EECProjTransferUnbinnedProducer;
typedef EECTransferProducer<EEC::ProjTransferCalculator, 
                               EEC::ProjResult_Array, 
                               EEC::ProjTransferResult_Array,
                               EEC::ProjCalculator> EECProjTransferArrayProducer;

DEFINE_FWK_MODULE(EECProjTransferVectorProducer);
DEFINE_FWK_MODULE(EECProjTransferUnbinnedProducer);
DEFINE_FWK_MODULE(EECProjTransferArrayProducer);
