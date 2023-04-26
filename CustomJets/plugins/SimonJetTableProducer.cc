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

#include "SRothman/Matching/src/toyjets/common.h"

#include <iostream>
#include <memory>
#include <vector>

class SimonJetTableProducer : public edm::stream::EDProducer<> {
public:
    explicit SimonJetTableProducer(const edm::ParameterSet&);
    ~SimonJetTableProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    std::string name_;
    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<jet>> srcToken_;

};

SimonJetTableProducer::SimonJetTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<jet>>(src_)){
    produces<nanoaod::FlatTable>(name_);
    produces<nanoaod::FlatTable>(name_+"BK");
}

void SimonJetTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("src");
  descriptions.addWithDefaultLabel(desc);
}

void SimonJetTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<jet>> jets;
  evt.getByToken(srcToken_, jets);

  std::vector<double>  partPt;
  std::vector<double> partEta;
  std::vector<double> partPhi;
  std::vector<int> pdgid;
  std::vector<int> charge;

  std::vector<double> pt;
  std::vector<double> eta;
  std::vector<double> phi;
  std::vector<int> iJet;
  std::vector<int> nPart;

  for(const auto& j : *jets){
      pt.push_back(j.pt);
      eta.push_back(j.eta);
      iJet.push_back(j.iJet);
      nPart.push_back(j.nPart);

      for(const auto& p : j.particles){
          partPt.push_back(p.pt);
          partEta.push_back(p.eta);
          partPhi.push_back(p.phi);
          pdgid.push_back(p.pdgid);
          charge.push_back(p.charge);
      }
  }

  auto table = std::make_unique<nanoaod::FlatTable>(partPt.size(), name_, false);
  table->addColumn<double>("pt", partPt, "particle pt");
  table->addColumn<double>("eta", partEta, "particle eta");
  table->addColumn<double>("phi", partPhi, "particle phi");
  table->addColumn<int>("pdgid", pdgid, "particle pdgid");
  table->addColumn<int>("charge", charge, "particle charge");
  evt.put(std::move(table), name_);

  auto tableBK = std::make_unique<nanoaod::FlatTable>(pt.size(), name_+"BK", false);
  tableBK->addColumn<double>("jetPt", pt, "jet pt");
  tableBK->addColumn<double>("jetEta", eta, "jet eta");
  tableBK->addColumn<double>("jetPhi", phi, "jet phi");
  tableBK->addColumn<int>("iJet", iJet, "index in primary jet array");
  tableBK->addColumn<int>("nPart", nPart, "number of particles in jet");
  evt.put(std::move(tableBK), name_+"BK");
}

DEFINE_FWK_MODULE(SimonJetTableProducer);
