#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
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

#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"

#include "SRothman/Analysis/src/RoccoR.h"

template <typename JET>
class JetSelectionFlagProducerT : public edm::stream::EDProducer<>{
public:
    explicit JetSelectionFlagProducerT(const edm::ParameterSet&);
    ~JetSelectionFlagProducerT() override { }

private:
    void produce(edm::Event&, const edm::EventSetup&) override;

    edm::InputTag jetTag_;
    edm::EDGetTokenT<edm::View<JET> > jetToken_;

    std::string cut_;
    StringCutObjectSelector<JET> cutSelector_;

    int verbose_;
};

template <typename JET>
JetSelectionFlagProducerT<JET>::JetSelectionFlagProducerT(const edm::ParameterSet& iConfig):
    jetTag_(iConfig.getParameter<edm::InputTag>("src")),
    jetToken_(consumes<edm::View<JET> >(jetTag_)),
    cut_(iConfig.getParameter<std::string>("cut")),
    cutSelector_(cut_),
    verbose_(iConfig.getParameter<int>("verbose"))
{
    produces<edm::ValueMap<bool> >();
}

template <typename JET>
void JetSelectionFlagProducerT<JET>::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
    edm::Handle<edm::View<JET> > jets;
    iEvent.getByToken(jetToken_, jets);

    std::vector<bool> flags(jets->size(), false);

    for(size_t i = 0; i < jets->size(); ++i){
        const JET& jet = jets->at(i);
        flags[i] = cutSelector_(jet);
    }

    auto out = std::make_unique<edm::ValueMap<bool>>();
    edm::ValueMap<bool>::Filler filler(*out);
    filler.insert(jets, flags.begin(), flags.end());
    filler.fill();

    iEvent.put(std::move(out));
}

#include "DataFormats/PatCandidates/interface/Jet.h"
typedef JetSelectionFlagProducerT<pat::Jet> PATJetSelectionFlagProducer;
DEFINE_FWK_MODULE(PATJetSelectionFlagProducer);
