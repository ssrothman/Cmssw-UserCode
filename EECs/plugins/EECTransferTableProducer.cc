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
    produces<nanoaod::FlatTable>(name_+"res4shapes");
    produces<nanoaod::FlatTable>(name_+"res4fixed");
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
  std::vector<float> trans4shapes;

  std::vector<float> trans4fixed;

  //BK
  std::vector<int> iReco, iGen;

  std::vector<int> nproj;
  std::vector<int> nres3_RL, nres3_xi, nres3_phi;
  std::vector<int> nres4shapes_RL, nres4shapes_shape, nres4shapes_r, nres4shapes_ct;
  std::vector<int> nres4fixed_RL, nres4fixed_shape;

  for(const auto& T : *Ts){
      iReco.push_back(T.iReco);
      iGen.push_back(T.iGen);

      unsigned Np = T.proj[0]->num_elements();
      for(unsigned i=0; i<5; ++i){
          if(i+2 <= T.maxOrder){
              flattenMultiArray(*(T.proj[i]), transP[i]);
          } else {
              transP[i].insert(transP[i].end(), Np, 0);
          }
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
          const auto& res4 = *(T.res4shapes);
          
          unsigned Nshape = res4.shape()[0];
          unsigned nRL = res4.shape()[1];
          unsigned nr = res4.shape()[2];
          unsigned nct = res4.shape()[3];

          flattenMultiArray(res4, trans4shapes);

          nres4shapes_RL.emplace_back(nRL);
          nres4shapes_shape.emplace_back(Nshape);
          nres4shapes_r.emplace_back(nr);
          nres4shapes_ct.emplace_back(nct);
      } else {
          nres4shapes_RL.emplace_back(0);
          nres4shapes_shape.emplace_back(0);
          nres4shapes_r.emplace_back(0);
          nres4shapes_ct.emplace_back(0);
      }

      if (T.doRes4Fixed){
          const auto& res4 = *(T.res4fixed);

          unsigned Nshape = res4.shape()[0];
          unsigned nRL = res4.shape()[1];

          flattenMultiArray(res4, trans4shapes);

          nres4fixed_RL.emplace_back(nRL);
          nres4fixed_shape.emplace_back(Nshape);
      } else {
          nres4fixed_RL.emplace_back(0);
          nres4fixed_shape.emplace_back(0);
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

  auto tableRes4shapes = std::make_unique<nanoaod::FlatTable>(trans4shapes.size(), name_+"res4shapes", false);
  tableRes4shapes->addColumn<float>("value", trans4shapes, "res4 transfer", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes4shapes), name_+"res4shapes");

  auto tableRes4fixed = std::make_unique<nanoaod::FlatTable>(trans4fixed.size(), name_+"res4fixed", false);
  tableRes4fixed->addColumn<float>("value", trans4fixed, "res4 transfer", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRes4fixed), name_+"res4fixed");

  auto tableBK = std::make_unique<nanoaod::FlatTable>(iReco.size(), name_+"BK", false);
  tableBK->addColumn<int>("iReco", iReco, "index of reco jet", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("iGen", iGen, "index of gen jet", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nproj", nproj, "number of projected EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres3_RL", nres3_RL, "number of res3 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres3_xi", nres3_xi, "number of res3 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres3_phi", nres3_phi, "number of res3 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4shapes_RL", nres4shapes_RL, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4shapes_shape", nres4shapes_shape, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4shapes_r", nres4shapes_r, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4shapes_ct", nres4shapes_ct, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4fixed_RL", nres4fixed_RL, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nres4fixed_shape", nres4fixed_shape, "number of res4 EEC weights", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableBK), name_+"BK");
}

DEFINE_FWK_MODULE(EECTransferTableProducer);
