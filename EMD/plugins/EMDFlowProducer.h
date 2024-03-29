#ifndef EMDFLOWPRODUCER_H
#define EMDFLOWPRODUCER_H

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


#include "SRothman/EMD/src/wasserstein/Wasserstein.hh"

#include <iostream>
#include <memory>
#include <vector>

#define MAX_CONSTITUENTS 255

#define VERBOSE 

using EMD = emd::EMDFloat64<emd::EuclideanEvent2D, emd::YPhiParticleDistance>;
using EMDParticle = emd::EuclideanParticle2D<double>;
using jetConstituents = std::vector<EMDParticle>;

class EMDFlowProducer : public edm::stream::EDProducer<> {
public:
  explicit EMDFlowProducer(const edm::ParameterSet&);
  ~EMDFlowProducer() override {}
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

  std::string mode_;

  EMD emd_obj_;

  template <typename T>
  double getConstituents_(T& jet, jetConstituents& out);
};

template <typename T>
double EMDFlowProducer::getConstituents_(T& jet, jetConstituents& out) {
  out.clear();

  std::vector<reco::Jet::Constituent> constituents = jet.getJetConstituents();
  size_t nConstituents = std::min<size_t>(constituents.size(), MAX_CONSTITUENTS);

  double rawpt = 0;
  double rawpt2 = 0;
  for(size_t iPart=0; iPart < nConstituents; ++iPart){
    auto part = constituents[iPart];
    double nextpt = 0;
    if(mode_ == "Ewt" || mode_ == "match"){
      nextpt = part->pt();
    } else if(mode_ == "nowt"){
      nextpt += 1.;
    } else {
      throw cms::Exception("EMDFlowProducer") << "Invalid mode" << std::endl;
    }
    rawpt += nextpt;
    if(part->pt() > minPartPt_){
      rawpt2 += nextpt;
    }
  }
  for(size_t iPart=0; iPart < nConstituents; ++iPart){
    auto part = constituents[iPart];

    double wt;
    if(part->pt() < minPartPt_){
      wt = 0.0;
    } else if(mode_ == "Ewt" || mode_ == "match"){
      wt = part->pt()/rawpt;
    } else if(mode_ == "nowt"){
      wt = 1.0/rawpt;
    }
    out.emplace_back(wt, part->eta(), part->phi());
  }

  return rawpt2/rawpt;
}


#endif
