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

#include <memory>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string>

template <class Calculator, class ResultType>
class EECProducer : public edm::stream::EDProducer<> {
public:
    explicit EECProducer(const edm::ParameterSet&);
    ~EECProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    Calculator calc_;

    edm::EDGetTokenT<edm::View<simon::jet>> jetsToken_;

    std::vector<edm::EDGetTokenT<edm::ValueMap<bool>>> flagsTokens_;
};

template <class Calculator, class ResultType>
EECProducer<Calculator, ResultType>::EECProducer(const edm::ParameterSet& conf) :
        calc_(conf.getParameter<edm::ParameterSet>("calculator")),
        jetsToken_(consumes<edm::View<simon::jet>>(conf.getParameter<edm::InputTag>("jets"))) {

    std::vector<edm::InputTag> flagsTags = conf.getParameter<std::vector<edm::InputTag>>("flags");
    flagsTokens_.reserve(flagsTags.size());
    for(const auto& tag : flagsTags){
        flagsTokens_.push_back(consumes<edm::ValueMap<bool>>(tag));
    }

    produces<std::vector<EEC::CMSSWResult<ResultType>>>("reco");
}

template <class Calculator, class ResultType>
void EECProducer<Calculator, ResultType>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    edm::ParameterSetDescription calc_desc;
    Calculator::fillPSetDescription(calc_desc);
    desc.add<edm::ParameterSetDescription>("calculator", calc_desc);

    desc.add<edm::InputTag>("jets");

    desc.add<std::vector<edm::InputTag>>("flags");

    descriptions.addWithDefaultLabel(desc);
}

template <class Calculator, class ResultType>
void EECProducer<Calculator, ResultType>::produce(edm::Event& event, const edm::EventSetup& setup) {
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

    auto result = std::make_unique<std::vector<EEC::CMSSWResult<ResultType>>>();
    result->reserve(jets->size());

    for (unsigned iJet = 0; iJet < jets->size(); ++iJet){
        if (!passAllFlags[iJet]) continue;

        const auto& jet = jets->at(iJet);
        result->emplace_back(iJet, iJet, calc_);
        calc_.compute_precomputed(jet, result->back().result);
    }
    event.put(std::move(result), "reco");
}

#include "SRothman/EECs/src/Res4Calculator.h"
#include "SRothman/EECs/src/Res4Result.h"

typedef EECProducer<EEC::Res4Calculator, 
                       EEC::Res4Result_Vector> EECRes4VectorProducer;
typedef EECProducer<EEC::Res4Calculator, 
                       EEC::Res4Result_Unbinned> EECRes4UnbinnedProducer;
typedef EECProducer<EEC::Res4Calculator, 
                       EEC::Res4Result_MultiArray> EECRes4ArrayProducer;

DEFINE_FWK_MODULE(EECRes4VectorProducer);
DEFINE_FWK_MODULE(EECRes4UnbinnedProducer);
DEFINE_FWK_MODULE(EECRes4ArrayProducer);

#include "SRothman/EECs/src/Res3Calculator.h"
#include "SRothman/EECs/src/Res3Result.h"

typedef EECProducer<EEC::Res3Calculator, 
                       EEC::Res3Result_Vector> EECRes3VectorProducer;
typedef EECProducer<EEC::Res3Calculator, 
                       EEC::Res3Result_Unbinned> EECRes3UnbinnedProducer;
typedef EECProducer<EEC::Res3Calculator, 
                       EEC::Res3Result_MultiArray> EECRes3ArrayProducer;

DEFINE_FWK_MODULE(EECRes3VectorProducer);
DEFINE_FWK_MODULE(EECRes3UnbinnedProducer);
DEFINE_FWK_MODULE(EECRes3ArrayProducer);


#include "SRothman/EECs/src/ProjCalculator.h"
#include "SRothman/EECs/src/ProjResult.h"

typedef EECProducer<EEC::ProjCalculator, 
                       EEC::ProjResult_Vector> EECProjVectorProducer;
typedef EECProducer<EEC::ProjCalculator, 
                       EEC::ProjResult_Unbinned> EECProjUnbinnedProducer;
typedef EECProducer<EEC::ProjCalculator, 
                       EEC::ProjResult_Array> EECProjArrayProducer;

DEFINE_FWK_MODULE(EECProjVectorProducer);
DEFINE_FWK_MODULE(EECProjUnbinnedProducer);
DEFINE_FWK_MODULE(EECProjArrayProducer);
