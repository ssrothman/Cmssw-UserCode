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
#include "SRothman/SimonTools/src/flattenMultiArray.h"

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
};

EECTransferTableProducer::EECTransferTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<EECtransfer>>(src_)),
          verbose_(conf.getParameter<int>("verbose")){
    produces<nanoaod::FlatTable>(name_+"proj");
    produces<nanoaod::FlatTable>(name_+"res3");
    produces<nanoaod::FlatTable>(name_+"res4dipole");
    produces<nanoaod::FlatTable>(name_+"res4tee");
    produces<nanoaod::FlatTable>(name_+"BK");
}

void EECTransferTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("src");
  desc.add<int>("verbose");
  descriptions.addWithDefaultLabel(desc);
}

void EECTransferTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<EECtransfer>> Ts;
  evt.getByToken(srcToken_, Ts);

  //proj
  std::array<std::vector<float>, 5> transP;

  //res3
  std::vector<float> trans3;

  //res4
  std::vector<float> trans4dipole;
  std::vector<float> trans4tee;

  //BK
  std::vector<int> iReco, iGen;
  std::vector<int> nproj;
  std::vector<int> nres3_RL, nres3_xi, nres3_phi;
  std::vector<int> nres4_dipole_RL, nres4_dipole_r, nres4_dipole_ct;
  std::vector<int> nres4_tee_RL, nres4_tee_r, nres4_tee_ct;

  for(const auto& T : *Ts){
      iReco.push_back(T.iReco);
      iGen.push_back(T.iGen);

      for(unsigned i=0; i<5; ++i){
          flattenMultiArray(*(T.proj[i]), transP[i]);
      }
      nproj.push_back(T.proj[0]->shape()[0]);

      if (T.doRes3){
          const auto& res3 = *(T.res3);

          unsigned nRL = res3.shape()[0];
          unsigned nxi = res3.shape()[1];
          unsigned nphi = res3.shape()[2];

          flattenMultiArray(res3, trans3);

          nres3_RL.emplace_back(nRL);
          nres3_xi.emplace_back(nxi);
          nres3_phi.emplace_back(nphi);
      } else{
          nres3_RL.emplace_back(0);
          nres3_xi.emplace_back(0);
          nres3_phi.emplace_back(0);
      }

      if(T.doRes4Shapes){
          const auto& dipole = *(T.res4shapes->dipole);
          
          unsigned nRL_dipole = dipole.shape()[0];
          unsigned nr_dipole = dipole.shape()[1];
          unsigned nct_dipole = dipole.shape()[2];

          flattenMultiArray(dipole, trans4dipole);

          nres4_dipole_RL.emplace_back(nRL_dipole);
          nres4_dipole_r.emplace_back(nr_dipole);
          nres4_dipole_ct.emplace_back(nct_dipole);

          const auto& tee = *(T.res4shapes->tee);

          unsigned nRL_tee = tee.shape()[0];
          unsigned nr_tee = tee.shape()[1];
          unsigned nct_tee = tee.shape()[2];

          flattenMultiArray(tee, trans4tee);

          nres4_tee_RL.emplace_back(nRL_tee);
          nres4_tee_r.emplace_back(nr_tee);
          nres4_tee_ct.emplace_back(nct_tee);
      } else {
          nres4_dipole_RL.emplace_back(0);
          nres4_dipole_r.emplace_back(0);
          nres4_dipole_ct.emplace_back(0);
          nres4_tee_RL.emplace_back(0);
          nres4_tee_r.emplace_back(0);
          nres4_tee_ct.emplace_back(0);
      }
  }

  auto tableproj = std::make_unique<nanoaod::FlatTable>(transP[0].size(), name_+"proj", false);
  for(unsigned i=0; i<5; ++i){
    tableproj->addColumn<float>(vformat("value%d",i+2), transP[i], "projected EEC transfer", nanoaod::FlatTable::FloatColumn);
  }
  evt.put(std::move(tableproj), name_+"proj");

  auto tableRes3 = std::make_unique<nanoaod::FlatTable>(trans3.size(), name_+"res3", false);
  tableRes3->addColumn<float>("value", trans3, "res3 transfer", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes3), name_+"res3");

  auto tableRes4dipole = std::make_unique<nanoaod::FlatTable>(trans4dipole.size(), name_+"res4dipole", false);
  tableRes4dipole->addColumn<float>("value", trans4dipole, "res4dipole transfer", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes4dipole), name_+"res4dipole");

  auto tableRes4tee = std::make_unique<nanoaod::FlatTable>(trans4tee.size(), name_+"res4tee", false);
  tableRes4tee->addColumn<float>("value", trans4tee, "res4tee transfer", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes4tee), name_+"res4tee");

  auto tableBK = std::make_unique<nanoaod::FlatTable>(iReco.size(), name_+"BK", false);
  tableBK->addColumn<int>("iReco", iReco, "index of reco jet", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("iGen", iGen, "index of gen jet", nanoaod::FlatTable::IntColumn);
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

DEFINE_FWK_MODULE(EECTransferTableProducer);
