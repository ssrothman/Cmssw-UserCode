#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "DataFormats/Common/interface/ValueMap.h"

template <typename SourceT, typename TargetT, typename Mapped>
class ValueMapTranslatorT : public edm::stream::EDProducer<> {
public:
    explicit ValueMapTranslatorT(const edm::ParameterSet&);
    ~ValueMapTranslatorT() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<SourceT>> srcToken_;

    edm::InputTag target_;
    edm::EDGetTokenT<edm::View<TargetT>> targetToken_;

    edm::InputTag map_;
    edm::EDGetTokenT<edm::ValueMap<Mapped>> mapToken_;

    int verbose_;
};

template <typename SourceT, typename TargetT, typename Mapped>
ValueMapTranslatorT<SourceT, TargetT, Mapped>::ValueMapTranslatorT(const edm::ParameterSet& conf)
        : src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<SourceT>>(src_)),
          target_(conf.getParameter<edm::InputTag>("target")),
          targetToken_(consumes<edm::View<TargetT>>(target_)),
          map_(conf.getParameter<edm::InputTag>("map")),
          mapToken_(consumes<edm::ValueMap<Mapped>>(map_)),
          verbose_(conf.getParameter<int>("verbose"))
{
    produces<edm::ValueMap<Mapped>>();
}

template <typename SourceT, typename TargetT, typename Mapped>
void ValueMapTranslatorT<SourceT, TargetT, Mapped>::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
    edm::Handle<edm::View<SourceT>> src;
    iEvent.getByToken(srcToken_, src);

    edm::Handle<edm::View<TargetT>> target;
    iEvent.getByToken(targetToken_, target);

    edm::Handle<edm::ValueMap<Mapped>> themap;
    iEvent.getByToken(mapToken_, themap);

    std::vector<Mapped> values(target->size());

    for(size_t i = 0; i < target->size(); ++i){
        const auto& t = target->at(i);
        const auto& s = src->refAt(t.iJet);
        values[i] = (*themap)[s];
    }
    
    auto out = std::make_unique<edm::ValueMap<Mapped>>();
    typename edm::ValueMap<Mapped>::Filler filler(*out);
    filler.insert(target, values.begin(), values.end());
    filler.fill();

    iEvent.put(std::move(out));
}

template <typename SourceT, typename TargetT, typename Mapped>
void ValueMapTranslatorT<SourceT, TargetT, Mapped>::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src");
    desc.add<edm::InputTag>("target");
    desc.add<edm::InputTag>("map");
    desc.add<int>("verbose");
    descriptions.addWithDefaultLabel(desc);
}

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "SRothman/SimonTools/src/jet.h"

typedef ValueMapTranslatorT<pat::Jet, simon::jet, bool> JetSelectionFlagTranslator;
DEFINE_FWK_MODULE(JetSelectionFlagTranslator);
