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
#include "SRothman/SimonTools/src/util.h"

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

    int verbose_;

    std::vector<int> orders_;

};

EECTableProducer::EECTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<EECresult>>(src_)),
          verbose_(conf.getParameter<int>("verbose")),
          orders_(conf.getParameter<std::vector<int>>("orders")){
    produces<nanoaod::FlatTable>(name_+"proj");
    produces<nanoaod::FlatTable>(name_+"res3");
    produces<nanoaod::FlatTable>(name_+"res4");
    produces<nanoaod::FlatTable>(name_+"BK");
}

void EECTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("src");
  desc.add<int>("verbose");
  desc.add<std::vector<int>>("orders");
  descriptions.addWithDefaultLabel(desc);
}

void EECTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  if(verbose_){
    printf("top of EECTableProducer::produce()\n");
    fflush(stdout);
  }
  edm::Handle<edm::View<EECresult>> EECs;
  evt.getByToken(srcToken_, EECs);

  std::vector<std::vector<float>> projwts;
  std::vector<std::vector<float>> projcov;

  std::vector<float> res3wts;
  std::vector<float> res3x3cov;
  std::vector<std::vector<float>> res3xPcov;

  std::vector<float> res4wts;
  std::vector<float> res4x4cov;
  std::vector<float> res4x3cov;
  std::vector<std::vector<float>> res4xPcov;

  std::vector<int> iJet, iReco;
  std::vector<int> nprojwts;
  std::vector<int> nprojcov;
  std::vector<int> nres3wts;
  std::vector<int> nres3x3cov;
  std::vector<int> nres3xPcov;
  std::vector<int> nres4wts;
  std::vector<int> nres4x4cov;
  std::vector<int> nres4x3cov;
  std::vector<int> nres4xPcov;

  projwts.resize(orders_.size());
  projcov.resize(orders_.size());
  res3xPcov.resize(orders_.size());
  res4xPcov.resize(orders_.size());

  for(const auto& EEC : *EECs){
      iJet.push_back(EEC.iJet);
      iReco.push_back(EEC.iReco);

      for(unsigned i=0; i<EEC.wts.size(); ++i){
          projwts[i].insert(projwts[i].end(), EEC.wts[i].begin(), EEC.wts[i].end());
      }
      nprojwts.emplace_back(EEC.wts[0].size());

      res3wts.insert(res3wts.end(), EEC.res3wts.begin(), EEC.res3wts.end());
      nres3wts.emplace_back(EEC.res3wts.size());

      res4wts.insert(res4wts.end(), EEC.res4wts.begin(), EEC.res4wts.end());
      nres4wts.emplace_back(EEC.res4wts.size());
  }

  auto table = std::make_unique<nanoaod::FlatTable>(projwts[0].size(), name_+"proj", false);
  for(unsigned i=0; i<orders_.size(); ++i){
    table->addColumn<float>(vformat("value%d", orders_[i]), projwts[i], "projected EEC weights", nanoaod::FlatTable::FloatColumn);
  }
  evt.put(std::move(table), name_+"proj");

  auto tableRes3 = std::make_unique<nanoaod::FlatTable>(res3wts.size(), name_+"res3", false);
  tableRes3->addColumn<float>("value", res3wts, "res3 EEC weights", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes3), name_+"res3");

  auto tableRes4 = std::make_unique<nanoaod::FlatTable>(res4wts.size(), name_+"res4", false);
  tableRes4->addColumn<float>("value", res4wts, "res4 EEC weights", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes4), name_+"res4");

  auto tableBK = std::make_unique<nanoaod::FlatTable>(iJet.size(), name_+"BK", false);
  tableBK->addColumn<int>("iJet", iJet, "index of jet", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("iReco", iReco, "index of jet", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nproj", nprojwts, "number of projected EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres3", nres3wts, "number of res3 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4", nres4wts, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableBK), name_+"BK");

}

DEFINE_FWK_MODULE(EECTableProducer);
