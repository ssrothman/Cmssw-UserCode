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

#include <iostream>
#include <memory>
#include <vector>

#include "TH1F.h"

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
    //produces<edm::ValueMap<std::vector<float>>>("dR"+std::to_string(i));
    //produces<edm::ValueMap<std::vector<float>>>("wt"+std::to_string(i));
    produces<edm::ValueMap<TH1F>>("EEC" + std::to_string(i));
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

  std::vector<std::vector<std::vector<float>>> allDRs;
  std::vector<std::vector<std::vector<float>>> allWTs;
  std::vector<std::vector<TH1F>> histograms;

  allDRs.resize(maxOrder_-1);
  allWTs.resize(maxOrder_-1);
  histograms.resize(maxOrder_-1);

  for(unsigned i=0; i<maxOrder_-1; ++i){
    allDRs[i].reserve(nJets);
    histograms[i].reserve(nJets);
  }

  for(size_t iJet=0; iJet<nJets; ++iJet){
    reco::PFJet jet = jets->at(iJet);
    std::vector<reco::PFCandidatePtr> constituents = jet.getPFConstituents();
    size_t nConstituents = constituents.size();
    for(unsigned order=2; order<maxOrder_; ++order){ 
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
      auto histo = std::make_unique<TH1F>("EEC","EEC",1000,0,1);
      for(size_t nR=0; nR<nDR; ++nR){
        histo->Fill(dRs[i], wts[i]);
      }
      histograms[order].push_back(*histo);
    } // end for order
  } // for for jet 
  
  for(unsigned order=2; order<=maxOrder_; ++order){
    
    auto outHists = std::make_unique<edm::ValueMap<TH1F>>();
    edm::ValueMap<TH1F>::Filler fillerHists(*outHists);
    fillerHists.insert(jets, histograms[order-2].begin(), histograms[order-2].end());
    fillerHists.fill();
    evt.put(std::move(outHists), "EEC" + std::to_string(order));

    /*auto outDR = std::make_unique<edm::ValueMap<std::vector<float>>>();
    edm::ValueMap<std::vector<float>>::Filler fillerDR(*outDR);
    fillerDR.insert(jets, allDRs[order-2].begin(), allDRs[order-2].end());
    fillerDR.fill();
    evt.put(std::move(outDR), "dR" + std::to_string(order));
    */

    /*
    auto outWT = std::make_unique<edm::ValueMap<std::vector<float>>>();
    edm::ValueMap<std::vector<float>>::Filler fillerWT(*outWT);
    fillerWT.insert(jets, allWTs[order-2].begin(), allWTs[order-2].end());
    fillerWT.fill();
    evt.put(std::move(outWT), "wt" + std::to_string(order));
    */
  }
  
}// end produce()

DEFINE_FWK_MODULE(EECProducer);
