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

class EECTransferTableProducer : public edm::stream::EDProducer<> {
public:
    explicit EECTransferTableProducer(const edm::ParameterSet&);
    ~EECTransferTableProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    std::string name_;
    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<EECtransfer>> srcToken_;

    int verbose_;

    std::vector<int> orders_;

};

EECTransferTableProducer::EECTransferTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<EECtransfer>>(src_)),
          verbose_(conf.getParameter<int>("verbose")),
          orders_(conf.getParameter<std::vector<int>>("orders")){
    produces<nanoaod::FlatTable>(name_+"proj");
    produces<nanoaod::FlatTable>(name_+"res3");
    produces<nanoaod::FlatTable>(name_+"res4");
    produces<nanoaod::FlatTable>(name_+"BK");
}

void EECTransferTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("src");
  desc.add<int>("verbose");
  desc.add<std::vector<int>>("orders");
  descriptions.addWithDefaultLabel(desc);
}

void EECTransferTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<EECtransfer>> Ts;
  evt.getByToken(srcToken_, Ts);

  //proj
  std::vector<std::vector<float>> transP;

  //res3
  std::vector<float> trans3;

  //res4
  std::vector<float> trans4;

  //BK
  std::vector<int> iReco;
  std::vector<int> iGen;
  std::vector<int> nproj;
  std::vector<int> nres3;
  std::vector<int> nres4;

  transP.resize(orders_.size());

  for(const auto& T : *Ts){
      iReco.push_back(T.iReco);
      iGen.push_back(T.iGen);

      for(unsigned i=0; i<orders_.size(); ++i){
          transP[i].insert(transP[i].end(), T.proj[i].begin(), T.proj[i].end());
      }
      nproj.push_back(T.proj[0].n_cols);

      trans3.insert(trans3.end(), T.res3.begin(), T.res3.end());
      nres3.push_back(T.res3.n_cols);

      trans4.insert(trans4.end(), T.res4.begin(), T.res4.end());
      nres4.push_back(T.res4.n_cols);
  }

  auto table = std::make_unique<nanoaod::FlatTable>(transP[0].size(), name_+"proj", false);
  for(unsigned i=0; i<orders_.size(); ++i){
    table->addColumn<float>(vformat("value%d",orders_[i]), transP[i], "projected EEC transfer", nanoaod::FlatTable::FloatColumn);
  }
  evt.put(std::move(table), name_+"proj");

  auto tableRes3 = std::make_unique<nanoaod::FlatTable>(trans3.size(), name_+"res3", false);
  tableRes3->addColumn<float>("value", trans3, "res3 transfer", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes3), name_+"res3");

  auto tableRes4 = std::make_unique<nanoaod::FlatTable>(trans4.size(), name_+"res4", false);
  tableRes4->addColumn<float>("value", trans4, "res4 transfer", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes4), name_+"res4");

  auto tableBK = std::make_unique<nanoaod::FlatTable>(iReco.size(), name_+"BK", false);
  tableBK->addColumn<int>("iReco", iReco, "index of reco jet", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("iGen", iGen, "index of gen jet", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nproj", nproj, "number of projected EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres3", nres3, "number of res3 weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4", nres4, "number of res4 weights", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableBK), name_+"BK");
}

DEFINE_FWK_MODULE(EECTransferTableProducer);
