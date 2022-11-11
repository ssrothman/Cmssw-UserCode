#ifndef GENMATCHFITPRODUCER_H
#define GENMATCHFITPRODUCER_H

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

#include <iostream>
#include <memory>
#include <vector>

#define MAX_CONSTITUENTS 10

using vecptr = std::shared_ptr<std::vector<double>>;

class GenMatchFitProducer : public edm::stream::EDProducer<> {
public:
  explicit GenMatchFitProducer(const edm::ParameterSet&);
  ~GenMatchFitProducer() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  edm::InputTag jetsTag_;
  edm::EDGetTokenT<edm::View<pat::Jet>> jetsToken_;

  edm::InputTag genJetsTag_;
  edm::EDGetTokenT<edm::View<reco::GenJet>> genJetsToken_;

  double dR2cut_;
  double minPartPt_;
  double partDR2cut_;

  vecptr recoPT_, recoETA_, recoPHI_, 
         genPT_, genETA_, genPHI_,
         errPT_, errETA_, errPHI_;

  unsigned maxIter_;
  double feasCondition_;
  double startMu_;
  double startLambda_;
  double clipVal_;

  template <typename T>
  double getConstituents_(const T& jet, 
                          vecptr ptOut,
                          vecptr etaOut,
                          vecptr phiOut);
};

template <typename T>
double GenMatchFitProducer::getConstituents_(const T& jet, 
                                             vecptr ptOut,
                                             vecptr etaOut,
                                             vecptr phiOut){
  ptOut->clear();
  etaOut->clear();
  phiOut->clear();

  const std::vector<reco::Jet::Constituent> constituents = jet.getJetConstituents();
  size_t nConstituents = std::min<size_t>(constituents.size(), MAX_CONSTITUENTS);

  double rawpt = 0;
  double rawpt2 = 0;
  for(size_t iPart=0; iPart < nConstituents; ++iPart){
    auto part = constituents[iPart];

    double nextpt = part->pt();

    rawpt += nextpt;
    if(part->pt() > minPartPt_){
      rawpt2 += nextpt;
    }
  }

  double JECfactor = jet.pt()/rawpt;

  for(size_t iPart=0; iPart < nConstituents; ++iPart){
    auto part = constituents[iPart];


    if(part->pt() < minPartPt_){
      continue;
    } else {
      ptOut->emplace_back(part->pt() * JECfactor);
      etaOut->emplace_back(part->eta());
      phiOut->emplace_back(part->phi());
    }
  }

  return rawpt2/rawpt;
}

#endif
