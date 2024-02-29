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

#include "SRothman/DataFormats/interface/jets.h"
#include "SRothman/DataFormats/interface/matching.h"
#include "SRothman/SimonTools/src/isID.h"

#include <iostream>
#include <memory>
#include <vector>

class SVMatchTableProducer : public edm::stream::EDProducer<> {
public:
    explicit SVMatchTableProducer(const edm::ParameterSet&);
    ~SVMatchTableProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    std::string name_;
    
    edm::InputTag jetSrc_;
    edm::EDGetTokenT<edm::View<pat::Jet>> jetToken_;

    edm::InputTag SVSrc_;
    edm::EDGetTokenT<edm::PtrVector<reco::Candidate>> SVToken_;
    double DRthresh_;

};

SVMatchTableProducer::SVMatchTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          jetSrc_(conf.getParameter<edm::InputTag>("jets")),
          jetToken_(consumes<edm::View<pat::Jet>>(jetSrc_)),
          SVSrc_(conf.getParameter<edm::InputTag>("SVs")),
          SVToken_(consumes<edm::PtrVector<reco::Candidate>>(SVSrc_)),
          DRthresh_(conf.getParameter<double>("DRthresh")){
    produces<nanoaod::FlatTable>();
}

void SVMatchTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("jets");
  desc.add<edm::InputTag>("SVs");
  desc.add<double>("DRthresh");
  descriptions.addWithDefaultLabel(desc);
}

void SVMatchTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<pat::Jet>> jets;
  evt.getByToken(jetToken_, jets);

  edm::Handle<edm::PtrVector<reco::Candidate>> SVs;
  evt.getByToken(SVToken_, SVs);

  std::vector<int> SVidx;

  for(const auto& j : *jets){
      int idx = 9999;
      double minDR = 9999;
      for(const auto& sv : *SVs){
          double dR = reco::deltaR(j.eta(), j.phi(), sv->eta(), sv->phi());
          if(dR < minDR && dR < DRthresh_){
              minDR = dR;
              idx = sv.key();
          }
      }
      SVidx.push_back(idx);
  }

  auto table = std::make_unique<nanoaod::FlatTable>(SVidx.size(), name_, false);
  table->addColumn<int>("SVidx", SVidx, "index of matched SV", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(table));
}

DEFINE_FWK_MODULE(SVMatchTableProducer);
