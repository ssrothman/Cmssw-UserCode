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
#include "SRothman/SimonTools/src/lund2.h"

class DeltaPsiProducer : public edm::stream::EDProducer<>{
public:
    explicit DeltaPsiProducer(const edm::ParameterSet&);
    ~DeltaPsiProducer() override { }

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    void produce(edm::Event&, const edm::EventSetup&) override;

    edm::InputTag jetTag_;
    edm::EDGetTokenT<edm::View<simon::jet>> jetToken_;

    bool hardSide_;
    double zcut1_, zcut2_;

    int verbose_;
};

DeltaPsiProducer::DeltaPsiProducer(const edm::ParameterSet& iConfig):
    jetTag_(iConfig.getParameter<edm::InputTag>("src")),
    jetToken_(consumes<edm::View<simon::jet>>(jetTag_)),
    hardSide_(iConfig.getParameter<bool>("hardSide")),
    zcut1_(iConfig.getParameter<double>("zcut1")),
    zcut2_(iConfig.getParameter<double>("zcut2")),
    verbose_(iConfig.getParameter<int>("verbose"))
{
    produces<std::vector<simon::DoubleSplittingInfo>>();
}

void DeltaPsiProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src")->setComment("Input jet collection");
    desc.add<int>("verbose", 0)->setComment("Verbosity level");
    desc.add<bool>("hardSide", true)->setComment("Whether to follow the hard side of the splitting");
    desc.add<double>("zcut1", 0.1)->setComment("z cut for first splitting");
    desc.add<double>("zcut2", 0.1)->setComment("z cut for second splitting");
    descriptions.addWithDefaultLabel(desc);
}

void DeltaPsiProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
    edm::Handle<edm::View<simon::jet>> jets;
    iEvent.getByToken(jetToken_, jets);

    auto values = std::make_unique<std::vector<simon::DoubleSplittingInfo>>();
    values->reserve(jets->size());

    for(size_t i = 0; i < jets->size(); ++i){
        const auto& jet = jets->at(i);
        
        simon::LundDeclustered(
            jet,
            hardSide_,
            zcut1_,
            zcut2_,
            *values
        );
        
        if(verbose_ > 0){
            std::cout << "Jet " << i 
                      << ": pt=" << jet.pt 
                      << ", eta=" << jet.eta 
                      << ", phi=" << jet.phi 
                      << ", deltaPsi=" << values->at(i).deltaPsi_type1()
                      << std::endl;
        }
    }

    iEvent.put(std::move(values));
}

DEFINE_FWK_MODULE(DeltaPsiProducer);

// NanoAOD Simple Table Producer Template for simon::DoubleSplittingInfo
#include "PhysicsTools/NanoAOD/interface/SimpleFlatTableProducer.h"

typedef SimpleFlatTableProducer<simon::DoubleSplittingInfo> SimpleSplittingInfoFlatTableProducer;

DEFINE_FWK_MODULE(SimpleSplittingInfoFlatTableProducer);

