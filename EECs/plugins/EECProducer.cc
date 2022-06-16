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
#include "DataFormats/JetReco/interface/Jet.h"

#include "TH1.h"

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
  edm::EDGetTokenT<edm::View<reco::Jet>> srcToken_;
};

EECProducer::EECProducer(const edm::ParameterSet& conf)
      : maxOrder(conf.getParameter<unsigned int>("maxOrder")),
        src_(conf.getParameter<edm::InputTag>("jets")),
        srcToken_(consumes<edm::View<reco::Jet>>(src_)){
  
  produces<edm::ValueMap<TH1D>>();

}

void EECProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){

}

void EECProducer::produce(edm::Event& evt, const edm::EventSetup &setup){
  
}

DEFINE_FWK_MODULE(EECProducer);
