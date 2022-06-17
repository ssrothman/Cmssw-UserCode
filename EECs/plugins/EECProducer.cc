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

//this is a gross hack
//I need to figure out how to actually link against this
#include "SRothman/EECs/src/eec_back.cc" 

#include <memory>

#include <vector>

class EECProducer : public edm::stream::EDProducer<> {
public:
  explicit EECProducer(const edm::ParameterSet&);
  ~EECProducer() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  unsigned int maxOrder;
  edm::InputTag src_;
  edm::EDGetTokenT<edm::View<reco::PFJet>> srcToken_;
};

EECProducer::EECProducer(const edm::ParameterSet& conf)
      : maxOrder(conf.getParameter<unsigned int>("maxOrder")),
        src_(conf.getParameter<edm::InputTag>("jets")),
        srcToken_(consumes<edm::View<reco::PFJet>>(src_)){
  
  for(unsigned i=2; i<maxOrder; ++i){
    produces<edm::ValueMap<std::vector<float>>>();
    produces<edm::ValueMap<std::vector<float>>>();

  }
}

void EECProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){

}

void EECProducer::produce(edm::Event& evt, const edm::EventSetup &setup){
  edm::Handle<edm::View<reco::PFJet>> jets;
  evt.getByToken(srcToken_, jets);

  unsigned nJets = jets->size();

  std::vector<std::vector<std::vector<float>>> allDRs;
  std::vector<std::vector<std::vector<float>>> allWTs;

  allDRs.resize(maxOrder-1);
  allWTs.resize(maxOrder-1);

  for(unsigned i=0; i<maxOrder-1; ++i){
    allDRs[i].reserve(nJets);
  }

  for(size_t iJet=0; iJet<nJets; ++iJet){
    reco::PFJet jet = jets->at(iJet);
    std::vector<reco::PFCandidatePtr> constituents = jet.getPFConstituents();
    size_t nConstituents = constituents.size();
    for(unsigned order=2; order<maxOrder; ++order){ 
      size_t nDR = choose(nConstituents, order);
      float* dRs = (float*) malloc(sizeof(float)*nDR);
      float* wts = (float*) malloc(sizeof(float)*nDR);
      float* jetFeat = (float*) malloc(sizeof(float)*3*nConstituents);
      size_t i=0;
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


      allDRs[order].emplace_back(dRs, dRs+nDR);
      allWTs[order].emplace_back(wts, wts+nDR);
    } // end for order
  } // for for jet 
  
  for(unsigned order=2; order<maxOrder; ++order){
    
    auto outDR = std::make_unique<edm::ValueMap<std::vector<float>>>();
    edm::ValueMap<std::vector<float>>::Filler fillerDR(*outDR);
    fillerDR.insert(jets, allDRs[order].begin(), allDRs[order].end());
    fillerDR.fill();
    evt.put(std::move(outDR));

    auto outWT = std::make_unique<edm::ValueMap<std::vector<float>>>();
    edm::ValueMap<std::vector<float>>::Filler fillerWT(*outWT);
    fillerWT.insert(jets, allWTs[order].begin(), allWTs[order].end());
    fillerWT.fill();
    evt.put(std::move(outWT));
  }
  
}// end produce()

DEFINE_FWK_MODULE(EECProducer);
