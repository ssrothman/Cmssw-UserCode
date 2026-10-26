#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "DataFormats/NanoAOD/interface/FlatTable.h"

#include "SRothman/DataFormats/interface/jets.h"
#include "SRothman/DataFormats/interface/matching.h"
#include "SRothman/SimonTools/src/isID.h"

#include <iostream>
#include <memory>
#include <vector>

#include <Eigen/Dense>

class TriggerResultsTableProducer : public edm::stream::EDProducer<> {
public:
    explicit TriggerResultsTableProducer(const edm::ParameterSet&);
    ~TriggerResultsTableProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    std::string name_;
    
    edm::InputTag src_;
    edm::EDGetTokenT<edm::TriggerResults> triggerBitsToken_;
    
    int verbose_;
};

TriggerResultsTableProducer::TriggerResultsTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          triggerBitsToken_(consumes<edm::TriggerResults>(src_)),
          verbose_(conf.getParameter<int>("verbose")){

    produces<nanoaod::FlatTable>(name_);
}

void TriggerResultsTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<std::string>("name");
    desc.add<edm::InputTag>("src");
    desc.add<int>("verbose");
    descriptions.add("TriggerResultsTableProducer", desc);
}

void TriggerResultsTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of TriggerResultsTableProducer::produce()\n");
    }

    edm::Handle<edm::TriggerResults> triggerBits;
    evt.getByToken(triggerBitsToken_, triggerBits);

    const edm::TriggerNames& triggerNames = evt.triggerNames(*triggerBits);

    auto out = std::make_unique<nanoaod::FlatTable>(1, name_, true, false);
    for (unsigned int i=0; i<triggerBits->size(); ++i) {
        std::string name = triggerNames.triggerName(i);
        std::vector<int> accept({{triggerBits->accept(i)}});
        if(verbose_){
            printf("Trigger %s: %d\n", name.c_str(), static_cast<int>(accept[0]));
        }
        out->addColumn<int>(name, accept, "Trigger decision for " + name, nanoaod::FlatTable::IntColumn);
    }

    evt.put(std::move(out), name_);
}

DEFINE_FWK_MODULE(TriggerResultsTableProducer);
