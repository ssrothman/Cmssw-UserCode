#include "SRothman/DataFormats/interface/EMDFlow.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"

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

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

class EMDFlowTableProducer : public edm::stream::EDProducer<> {
public:
  explicit EMDFlowTableProducer(const edm::ParameterSet&);
  ~EMDFlowTableProducer() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  edm::InputTag flowsTag_;
  edm::EDGetTokenT<EMDFlowCollection> flowsToken_;
};

EMDFlowTableProducer::EMDFlowTableProducer(const edm::ParameterSet& conf)
    : flowsTag_(conf.getParameter<edm::InputTag>("flow")),
      flowsToken_(consumes<EMDFlowCollection>(flowsTag_)){
  produces<nanoaod::FlatTable>("EMDFlowTable");
}

void EMDFlowTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("flow");
  descriptions.addWithDefaultLabel(desc);
}

void EMDFlowTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<EMDFlowCollection> flows;
  evt.getByToken(flowsToken_, flows);

  std::vector<int> iGen;
  std::vector<int> iReco;
  std::vector<float> flowvec;

  for(auto flow : *flows){
    for(auto f: flow.flow){
      flowvec.emplace_back(f);
      iGen.emplace_back(flow.iGen);
      iReco.emplace_back(flow.iReco);
    }
  }

  auto table = std::make_unique<nanoaod::FlatTable>(flowvec.size(), "EMDFlow", false);
  table->addColumn<float>("Flows", flowvec, "Energy movers distance optimal flow between jet constituents", nanoaod::FlatTable::FloatColumn);
  table->addColumn<int>("genJetIdx", iGen, "gen jet index", nanoaod::FlatTable::IntColumn);
  table->addColumn<int>("recoJetIdx", iReco, "reco jet index", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(table), "EMDFlowTable");
  
}  // end produce()

DEFINE_FWK_MODULE(EMDFlowTableProducer);
