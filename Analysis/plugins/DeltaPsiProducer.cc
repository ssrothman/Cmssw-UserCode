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
    produces<edm::ValueMap<float>>();
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

    std::vector<float> values;
    values.reserve(jets->size());

    for(size_t i = 0; i < jets->size(); ++i){
        const auto& jet = jets->at(i);
        
        double dpsi = simon::get_dpsi(jet);
        
        if(verbose_ > 0){
            std::cout << "Jet " << i 
                      << ": pt=" << jet.pt 
                      << ", eta=" << jet.eta 
                      << ", phi=" << jet.phi 
                      << ", deltaPsi=" << dpsi 
                      << std::endl;
        }
        
        values.push_back(dpsi); // downcast to float because NanoAOD expects floats
    }

    std::unique_ptr<edm::ValueMap<float>> out = std::make_unique<edm::ValueMap<float>>();
    edm::ValueMap<float>::Filler filler(*out);
    filler.insert(jets, values.begin(), values.end());
    filler.fill();

    iEvent.put(std::move(out));
}

DEFINE_FWK_MODULE(DeltaPsiProducer);
