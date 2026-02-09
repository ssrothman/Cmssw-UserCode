#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"

#include "SRothman/SimonTools/src/jet.h"
#include "SRothman/SimonTools/src/lund.h"

class DeltaPsiProducer : public edm::stream::EDProducer<>{
public:
    explicit DeltaPsiProducer(const edm::ParameterSet&);
    ~DeltaPsiProducer() override { }

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    void produce(edm::Event&, const edm::EventSetup&) override;

    edm::InputTag jetTag_;
    edm::EDGetTokenT<edm::View<simon::jet>> jetToken_;

    int verbose_;
};

DeltaPsiProducer::DeltaPsiProducer(const edm::ParameterSet& iConfig):
    jetTag_(iConfig.getParameter<edm::InputTag>("src")),
    jetToken_(consumes<edm::View<simon::jet>>(jetTag_)),
    verbose_(iConfig.getParameter<int>("verbose"))
{
    produces<std::vector<simon::SplittingInfo>>();
}

void DeltaPsiProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src")->setComment("Input jet collection");
    desc.add<int>("verbose", 0)->setComment("Verbosity level");
    descriptions.addWithDefaultLabel(desc);
}

void DeltaPsiProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
    edm::Handle<edm::View<simon::jet>> jets;
    iEvent.getByToken(jetToken_, jets);

    auto values = std::make_unique<std::vector<simon::SplittingInfo>>();
    values->resize(jets->size());

    for(size_t i = 0; i < jets->size(); ++i){
        const auto& jet = jets->at(i);
        
        simon::hardest_splitting_info(
            jet,
            values->at(i)
        );
        
        if(verbose_ > 0){
            std::cout << "Jet " << i 
                      << ": pt=" << jet.pt 
                      << ", eta=" << jet.eta 
                      << ", phi=" << jet.phi 
                      << ", deltaPsi=" << values->at(i).deltaPsi
                      << std::endl;
        }
    }

    iEvent.put(std::move(values));
}

DEFINE_FWK_MODULE(DeltaPsiProducer);

// NanoAOD Simple Table Producer Template for simon::SplittingInfo
#include "PhysicsTools/NanoAOD/interface/SimpleFlatTableProducer.h"

typedef SimpleFlatTableProducer<simon::SplittingInfo> SimpleSplittingInfoFlatTableProducer;

DEFINE_FWK_MODULE(SimpleSplittingInfoFlatTableProducer);

