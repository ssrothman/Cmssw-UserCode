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

#include "EECObj.h"

//this is a gross hack
//I need to figure out how to actually link against this
#include "SRothman/EECs/src/eec_back.cc" 

#include <iostream>
#include <memory>
#include <vector>

class EECProducer : public edm::stream::EDProducer<> {
public:
  explicit EECProducer(const edm::ParameterSet&);
  ~EECProducer() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  unsigned int maxOrder_;
  edm::InputTag src_;
  edm::EDGetTokenT<edm::View<reco::PFJet>> srcToken_;
};

EECProducer::EECProducer(const edm::ParameterSet& conf)
      : maxOrder_(conf.getParameter<unsigned int>("maxOrder")),
        src_(conf.getParameter<edm::InputTag>("jets")),
        srcToken_(consumes<edm::View<reco::PFJet>>(src_)){
  
  for(unsigned i=2; i<=maxOrder_; ++i){
    produces<std::vector<float>>("dR"+std::to_string(i));
    produces<std::vector<float>>("wt"+std::to_string(i));
    produces<std::vector<unsigned>>("nDR"+std::to_string(i));
    //produces<std::vector<TH1F>>("EEC" + std::to_string(i));
    std::cout << "setting up to produce " << "dR" + std::to_string(i) << std::endl;
    std::cout << "setting up to produce " << "wt" + std::to_string(i) << std::endl;
    std::cout << "setting up to produce " << "nDR" + std::to_string(i) << std::endl;
  }
}

void EECProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
  edm::ParameterSetDescription desc;
  desc.add<unsigned int>("maxOrder", 2);
  desc.add<edm::InputTag>("jets");
  descriptions.addWithDefaultLabel(desc);
}

void EECProducer::produce(edm::Event& evt, const edm::EventSetup &setup){
  std::cout << "doing an event" << std::endl;
  edm::Handle<edm::View<reco::PFJet>> jets;
  evt.getByToken(srcToken_, jets);

  unsigned nJets = jets->size();

  for(unsigned order=2; order<maxOrder_; ++order){
    auto flatDRs = std::make_unique<std::vector<float>>();
    auto flatWTs = std::make_unique<std::vector<float>>();
    auto nDRs = std::make_unique<std::vector<unsigned>>();

    for(size_t iJet=0; iJet<nJets; ++iJet){
      reco::PFJet jet = jets->at(iJet);
      std::vector<reco::Jet::Constituent> constituents = jet.getJetConstituents();
      size_t nConstituents = constituents.size();

      size_t nDR = choose(nConstituents, order);
      float* dRs = (float*) malloc(sizeof(float)*nDR);
      float* wts = (float*) malloc(sizeof(float)*nDR);
      float* jetFeat = (float*) malloc(sizeof(float)*3*nConstituents);      size_t i=0;
      
      for(const auto& part : constituents){
        jetFeat[i++] = (float) part->pt();
        jetFeat[i++] = (float) part->eta();
        jetFeat[i++] = (float) part->phi();
      }

      eec_onejet(jetFeat, nConstituents, 3, 
                order,
                dRs, nDR,
                wts, nDR,
                6);

      flatDRs->insert(flatDRs->end(), dRs, dRs+nDR);
      flatWTs->insert(flatWTs->end(), wts, wts+nDR);
      nDRs->push_back(nDR);
    } // end for jet
    evt.put(std::move(flatDRs), "dR"+std::to_string(order));
    evt.put(std::move(flatWTs), "wt"+std::to_string(order));
    evt.put(std::move(nDRs), "nDR"+std::to_string(order));
    
    std::cout << "produced " << "dR" + std::to_string(order) << std::endl;
    std::cout << "produced " << "dR" + std::to_string(order) << std::endl;
    std::cout << "produced " << "dR" + std::to_string(order) << std::endl;
  } // end for order 
}// end produce()

DEFINE_FWK_MODULE(EECProducer);
