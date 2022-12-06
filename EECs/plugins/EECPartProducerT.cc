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
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "SRothman/EECs/src/eec.h"
#include "SRothman/EECs/src/combinatorics.h"
#include "SRothman/EECs/src/vecND.h"

#include "SRothman/DataFormats/interface/EEC.h"

#include <iostream>
#include <memory>
#include <vector>

#define MAX_CONSTITUENTS 128

#define VERBOSE 

template <typename T>
class EECPartProducerT : public edm::stream::EDProducer<> {
public:
  explicit EECPartProducerT(const edm::ParameterSet&);
  ~EECPartProducerT() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  double minPartPt_;

  unsigned int verbose_;

  edm::InputTag src_;
  edm::EDGetTokenT<edm::View<T>> srcToken_;

  edm::InputTag muonSrc_;
  edm::EDGetTokenT<edm::View<pat::Muon>> muonToken_;

  bool requireZ_;

};

template <typename T>
EECPartProducerT<T>::EECPartProducerT(const edm::ParameterSet& conf)
    : minPartPt_(conf.getParameter<double>("minPartPt")),
      verbose_(conf.getParameter<unsigned int>("verbose")),
      src_(conf.getParameter<edm::InputTag>("jets")),
      srcToken_(consumes<edm::View<T>>(src_)),
      muonSrc_(conf.getParameter<edm::InputTag>("muons")),
      muonToken_(consumes<edm::View<pat::Muon>>(muonSrc_)),
      requireZ_(conf.getParameter<bool>("requireZ")){
  produces<EECPartsCollection>();

}

template <typename T>
void EECPartProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<double>("minPartPt");
  desc.add<edm::InputTag>("jets");
  desc.add<unsigned int>("verbose");
  desc.add<edm::InputTag>("muons");
  desc.add<bool>("requireZ");
  descriptions.addWithDefaultLabel(desc);
}

template <typename T>
void EECPartProducerT<T>::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<T>> jets;
  evt.getByToken(srcToken_, jets);

  edm::Handle<edm::View<pat::Muon>> muons;
  evt.getByToken(muonToken_, muons);

  bool doEvent = true;

  unsigned nJets = jets->size();
  if(verbose_)
    std::cout << " Event has " << nJets << " jets" << std::endl;

  if(requireZ_){
    doEvent=false;
    if(muons->size() >= 2){
      if(verbose_){
        printf("muon1: (%0.3f, %0.3f, %0.3f)\n", muons->at(0).pt(), 
                                                 muons->at(0).eta(), 
                                                 muons->at(0).phi());
        printf("muon2: (%0.3f, %0.3f, %0.3f)\n", muons->at(1).pt(), 
                                                 muons->at(1).eta(), 
                                                 muons->at(1).phi());
      }

      doEvent=true;
    } else if(verbose_){
      std::cout << "skipping event because of too few muons" << std::endl;
    }
  }

  auto result = std::make_unique<EECPartsCollection>();

  for (size_t iJet = 0; iJet < nJets && doEvent; ++iJet) {

    T jet = jets->at(iJet);

    std::vector<reco::Jet::Constituent> constituents = jet.getJetConstituents();
    size_t nConstituents = std::min<size_t>(constituents.size(), MAX_CONSTITUENTS);

    if (nConstituents<2){ //skip jet
      continue;
    } 

    if (verbose_){
      std::cout << "\tjet: (" << jet.pt() << ", " << jet.eta() << ", " << jet.phi() << ")" << std::endl;
    }

    if(requireZ_){
      if constexpr (std::is_same<T, pat::Jet>::value){
        if(jet.muonEnergyFraction() > 0.7 || jet.chargedEmEnergyFraction() > 0.7){
          if(verbose_){
            std::cout << "\tSkipping because of lep veto energy fraction" << std::endl;
          }
          continue;
        }
      } else if constexpr (std::is_same<T, reco::GenJet>::value){
        if(jet.muonEnergy()/jet.pt() > 0.7 || jet.chargedEmEnergy()/jet.pt() > 0.7){
          if(verbose_){
            std::cout << "\tSkipping because of muon energy fraction" << std::endl;
          }
          continue;
        }
      }
    }

    //jet passed conditions, actually do the work now

    auto partPt = std::make_shared<std::vector<double>>(nConstituents);
    auto partEta = std::make_shared<std::vector<double>>(nConstituents);
    auto partPhi = std::make_shared<std::vector<double>>(nConstituents);
    auto partPdgId = std::make_shared<std::vector<int>>(nConstituents);

    double rawPt;

    //loop to compute rawPt
    for(size_t iPart=0; iPart<nConstituents; ++iPart){
      auto part = constituents[iPart];
      rawPt += part->pt();
    }

    //loop to fill vectors
    for(size_t iPart=0; iPart<nConstituents; ++iPart){
      auto part = constituents[iPart];
      if(part->pt() < minPartPt_){
        continue;
      }
      partPt->push_back(part->pt());
      partEta->push_back(part->eta());
      partPhi->push_back(part->phi());
      partPdgId->push_back(part->pdgId());
    } 

    result->emplace_back(iJet, rawPt, minPartPt_, 
        std::move(partPt), std::move(partEta), std::move(partPhi), 
        std::move(partPdgId));

  }  // end for jet
  evt.put(std::move(result));
}  // end produce()

typedef EECPartProducerT<reco::PFJet> EECPartsProducer;
typedef EECPartProducerT<reco::GenJet> GenEECPartsProducer;
typedef EECPartProducerT<pat::Jet> PatEECPartsProducer;

DEFINE_FWK_MODULE(EECPartsProducer);
DEFINE_FWK_MODULE(GenEECPartsProducer);
DEFINE_FWK_MODULE(PatEECPartsProducer);
