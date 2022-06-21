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

#include "DataFormats/NanoAOD/interface/FlatTable.h"

//this is a gross hack
//I need to figure out how to actually link against this
#include "SRothman/EECs/src/eec_back.cc" 

#include <iostream>
#include <memory>
#include <vector>

class EECTableProducer : public edm::stream::EDProducer<> {
public:
  explicit EECTableProducer(const edm::ParameterSet&);
  ~EECTableProducer() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  const unsigned int order_;
  const std::string name_;
  
  edm::InputTag src_;
  edm::EDGetTokenT<edm::View<reco::PFJet>> srcToken_;
};

EECTableProducer::EECTableProducer(const edm::ParameterSet& conf)
      : order_(conf.getParameter<unsigned int>("order")),
        name_(conf.getParameter<std::string>("name")),
        src_(conf.getParameter<edm::InputTag>("jets")),
        srcToken_(consumes<edm::View<reco::PFJet>>(src_)){
  produces<nanoaod::FlatTable>(name_);
}

void EECTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
  edm::ParameterSetDescription desc;
  desc.add<unsigned int>("order");
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("jets");
  descriptions.addWithDefaultLabel(desc);
}

void EECTableProducer::produce(edm::Event& evt, const edm::EventSetup &setup){
  std::cout << "doing an event" << std::endl;
  edm::Handle<edm::View<reco::PFJet>> jets;
  evt.getByToken(srcToken_, jets);

  unsigned nJets = jets->size();

  std::vector<float> flatDRs, flatWTs;
  std::vector<int> jetIdx;

  for(size_t iJet=0; iJet<nJets; ++iJet){
    std::cout << "\tJet " << iJet << std::endl;
    reco::PFJet jet = jets->at(iJet);
    std::vector<reco::Jet::Constituent> constituents = jet.getJetConstituents();
    size_t nConstituents = constituents.size();

    size_t nDR = choose(nConstituents, 2);
    float* dRs = (float*) malloc(sizeof(float)*nDR);
    float* wts = (float*) malloc(sizeof(float)*nDR);
    float* jetFeat = (float*) malloc(sizeof(float)*3*nConstituents);      size_t i=0;
    
    for(const auto& part : constituents){
      jetFeat[i++] = (float) part->pt();
      jetFeat[i++] = (float) part->eta();
      jetFeat[i++] = (float) part->phi();
    }

    eec_onejet(jetFeat, nConstituents, 3, 
              order_,
              dRs, nDR,
              wts, nDR,
              6);

    flatDRs.insert(flatDRs.end(), dRs, dRs+nDR);
    flatWTs.insert(flatWTs.end(), wts, wts+nDR);
    jetIdx.insert(jetIdx.end(), nDR, iJet);
  } // end for jet
  
  auto table = std::make_unique<nanoaod::FlatTable>(flatDRs.size(), name_, false);
  table->addColumn<float>("dRs", flatDRs, "Delta R", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("wts", flatWTs, "Weight", nanoaod::FlatTable::FloatColumn);
  table->addColumn<int>("jetIdx", jetIdx, "jet index", nanoaod::FlatTable::IntColumn);
  std::cout << "putting" << std::endl;
  evt.put(std::move(table), name_);
}// end produce()

DEFINE_FWK_MODULE(EECTableProducer);
