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
#include "SRothman/DataFormats/interface/EEC.h"

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
    std::string name_;
    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<EECresult>> srcToken_;

};

EECTableProducer::EECTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<EECresult>>(src_)){
    produces<nanoaod::FlatTable>(name_+"WTS");
    produces<nanoaod::FlatTable>(name_+"DRS");
    produces<nanoaod::FlatTable>(name_+"COV");
    produces<nanoaod::FlatTable>(name_+"WTSBK");
    produces<nanoaod::FlatTable>(name_+"BK");
}

void EECTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("src");
  descriptions.addWithDefaultLabel(desc);
}

void EECTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
    printf("top of produce\n");
  edm::Handle<edm::View<EECresult>> EECs;
  evt.getByToken(srcToken_, EECs);
  printf("got from event\n");

  std::vector<float> wts;
  std::vector<float> dRs;

  std::vector<float> cov;

  std::vector<int> offsets;
  std::vector<int> order;

  std::vector<int> iJet;
  std::vector<int> nOrders;
  std::vector<int> nWts;
  std::vector<int> nCov;
  std::vector<int> nDR;
  printf("made vectors\n");

  for(const auto& EEC : *EECs){
      iJet.push_back(EEC.iJet);
      nOrders.push_back(EEC.offsets.size());
      nWts.push_back(EEC.wts.size());
      nCov.push_back(EEC.cov.size());
      nDR.push_back(EEC.dRs.size());
      
      offsets.insert(offsets.end(), EEC.offsets.begin(), EEC.offsets.end());
      order.insert(order.end(), EEC.order.begin(), EEC.order.end());

      wts.insert(wts.end(), EEC.wts.begin(), EEC.wts.end());
      dRs.insert(dRs.end(), EEC.dRs.begin(), EEC.dRs.end());

      cov.insert(cov.end(), EEC.cov.begin(), EEC.cov.end());
  }

  auto tableWTS = std::make_unique<nanoaod::FlatTable>(wts.size(), name_+"WTS", false);
  tableWTS->addColumn<float>("value", wts, "EEC weights", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableWTS), name_+"WTS");

  auto tableDRs = std::make_unique<nanoaod::FlatTable>(dRs.size(), name_+"DRS", false);
  tableDRs->addColumn<float>("value", dRs, "EEC dRs", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableDRs), name_+"DRS");

  auto tableCOV = std::make_unique<nanoaod::FlatTable>(cov.size(), name_+"COV", false);
  tableCOV->addColumn<float>("value", cov, "EEC covariance matrix", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableCOV), name_+"COV");

  auto tableWTSBK = std::make_unique<nanoaod::FlatTable>(offsets.size(), name_+"WTSBK", false);
  tableWTSBK->addColumn<int>("offset", offsets, "offset into wts", nanoaod::FlatTable::IntColumn);
  tableWTSBK->addColumn<int>("order", order, "correlator order", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableWTSBK), name_+"WTSBK");

  auto tableBK = std::make_unique<nanoaod::FlatTable>(iJet.size(), name_+"BK", false);
  tableBK->addColumn<int>("iJet", iJet, "index in jet array", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nOrders", nOrders, "number of EEC orders", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nWts", nWts, "number of EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nCov", nCov, "number of covariance matrix elements", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nDR", nDR, "number of dR points", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableBK), name_+"BK");
}

DEFINE_FWK_MODULE(EECTableProducer);
