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
#include "SRothman/SimonTools/src/flattenMultiArray.h"

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
};

EECTableProducer::EECTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<EECresult>>(src_)),
          verbose_(conf.getParameter<int>("verbose")){
    produces<nanoaod::FlatTable>(name_+"proj");
    produces<nanoaod::FlatTable>(name_+"res3");
    produces<nanoaod::FlatTable>(name_+"res4dipole");
    produces<nanoaod::FlatTable>(name_+"res4tee");
    produces<nanoaod::FlatTable>(name_+"BK");
}

void EECTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("src");
  desc.add<int>("verbose");
  descriptions.addWithDefaultLabel(desc);
}

void EECTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  if(verbose_){
    printf("top of EECTableProducer::produce()\n");
    fflush(stdout);
  }
  edm::Handle<edm::View<EECresult>> EECs;
  evt.getByToken(srcToken_, EECs);

  std::array<std::vector<float>, 5> proj;
  std::vector<float> res3;
  std::vector<float> res4_dipole, res4_tee;

  std::vector<int> iJet, iReco;
  std::vector<int> nproj;
  std::vector<int> nres3_RL, nres3_xi, nres3_phi;
  std::vector<int> nres4_dipole_RL, nres4_dipole_r, nres4_dipole_ct;
  std::vector<int> nres4_tee_RL, nres4_tee_r, nres4_tee_ct;

  for(const auto& EEC : *EECs){
      iJet.push_back(EEC.iJet);
      iReco.push_back(EEC.iReco);

      for(unsigned i=0; i<5; ++i){
          proj[i].insert(proj[i].end(), EEC.proj[i]->begin(), EEC.proj[i]->end());
      }
      nproj.emplace_back(EEC.proj[0]->size());

      if (EEC.doRes3){
          const auto& EECres3 = *(EEC.res3);
          unsigned nRL = EECres3.shape()[0];
          unsigned nxi = EECres3.shape()[1];
          unsigned nphi = EECres3.shape()[2];

          flattenMultiArray(EECres3, res3);

          nres3_RL.emplace_back(nRL);
          nres3_xi.emplace_back(nxi);
          nres3_phi.emplace_back(nphi);
      } else {
          nres3_RL.emplace_back(0);
          nres3_xi.emplace_back(0);
          nres3_phi.emplace_back(0);
      }

      if (EEC.doRes4Shapes){
          const auto& dipole = *(EEC.res4shapes->dipole);
          nres4_dipole_RL.emplace_back(dipole.shape()[0]);
          nres4_dipole_r.emplace_back(dipole.shape()[1]);
          nres4_dipole_ct.emplace_back(dipole.shape()[2]);

          flattenMultiArray(dipole, res4_dipole);

          const auto& tee = *(EEC.res4shapes->tee);
          nres4_tee_RL.emplace_back(tee.shape()[0]);
          nres4_tee_r.emplace_back(tee.shape()[1]);
          nres4_tee_ct.emplace_back(tee.shape()[2]);

          flattenMultiArray(tee, res4_tee);
      } else {
          nres4_dipole_RL.emplace_back(0);
          nres4_dipole_r.emplace_back(0);
          nres4_dipole_ct.emplace_back(0);

          nres4_tee_RL.emplace_back(0);
          nres4_tee_r.emplace_back(0);
          nres4_tee_ct.emplace_back(0);
      }
  }

  auto tableproj = std::make_unique<nanoaod::FlatTable>(proj[0].size(), name_+"proj", false);
  for(unsigned i=0; i<5; ++i){
    tableproj->addColumn<float>(vformat("value%d", i+2), proj[i], "projected EEC weights", nanoaod::FlatTable::FloatColumn);
  }
  evt.put(std::move(tableproj), name_+"proj");

  auto tableRes3 = std::make_unique<nanoaod::FlatTable>(res3.size(), name_+"res3", false);
  tableRes3->addColumn<float>("value", res3, "res3 EEC weights", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes3), name_+"res3");

  auto tableRes4dipole = std::make_unique<nanoaod::FlatTable>(res4_dipole.size(), name_+"res4dipole", false);
  tableRes4dipole->addColumn<float>("value", res4_dipole, "res4 dipole weights", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes4dipole), name_+"res4dipole");

  auto tableRes4tee = std::make_unique<nanoaod::FlatTable>(res4_tee.size(), name_+"res4tee", false);
  tableRes4tee->addColumn<float>("value", res4_tee, "res4 tee weights", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes4tee), name_+"res4tee");

  auto tableBK = std::make_unique<nanoaod::FlatTable>(iJet.size(), name_+"BK", false);
  tableBK->addColumn<int>("iJet", iJet, "index of jet", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("iReco", iReco, "index of jet", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nproj", nproj, "number of projected EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres3_RL", nres3_RL, "number of res3 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres3_xi", nres3_xi, "number of res3 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres3_phi", nres3_phi, "number of res3 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4_dipole_RL", nres4_dipole_RL, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4_dipole_r", nres4_dipole_r, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4_dipole_ct", nres4_dipole_ct, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4_tee_RL", nres4_tee_RL, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4_tee_r", nres4_tee_r, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4_tee_ct", nres4_tee_ct, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableBK), name_+"BK");

}

DEFINE_FWK_MODULE(EECTableProducer);
