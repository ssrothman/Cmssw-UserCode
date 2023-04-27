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

};

EECTransferTableProducer::EECTransferTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<EECtransfer>>(src_)){
    produces<nanoaod::FlatTable>(name_+"PROJ");
    produces<nanoaod::FlatTable>(name_+"RES3");
    produces<nanoaod::FlatTable>(name_+"RES4");
    produces<nanoaod::FlatTable>(name_+"BK");
    produces<nanoaod::FlatTable>(name_+"PROJBK");
    produces<nanoaod::FlatTable>(name_+"RES3BK");
    produces<nanoaod::FlatTable>(name_+"RES4BK");
}

void EECTransferTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("src");
  descriptions.addWithDefaultLabel(desc);
}

void EECTransferTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<EECtransfer>> Ts;
  evt.getByToken(srcToken_, Ts);

  //proj
  std::vector<float> transP;

  //res3
  std::vector<float> trans3;

  //res4
  std::vector<float> trans4;

  //BK
  std::vector<int> iReco;
  std::vector<int> iGen;

  //projbk
  std::vector<int> nTransP;
  std::vector<int> nRecoP;
  std::vector<int> nGenP;
  std::vector<int> order;

  //res3bk
  std::vector<int> nTrans3;
  std::vector<int> nReco3;
  std::vector<int> nGen3;

  //res4bk
  std::vector<int> nTrans4;
  std::vector<int> nReco4;
  std::vector<int> nGen4;

  for(const auto& T : *Ts){
      iReco.push_back(T.iReco);
      iGen.push_back(T.iGen);
      for(unsigned i=0; i<T.proj.size(); ++i){
          const auto& m = T.proj.at(i);
          transP.insert(transP.end(), m.begin(), m.end());
          order.push_back(T.order.at(i));
          nTransP.push_back(m.size());
          nRecoP.push_back(m.n_rows);
          nGenP.push_back(m.n_cols);
      }

      trans3.insert(trans3.end(), T.res3.begin(), T.res3.end());
      nTrans3.push_back(T.res3.size());
      nReco3.push_back(T.res3.n_rows);
      nGen3.push_back(T.res3.n_cols);

      trans4.insert(trans4.end(), T.res4.begin(), T.res4.end());
      nTrans4.push_back(T.res4.size());
      nReco4.push_back(T.res4.n_rows);
      nGen4.push_back(T.res4.n_cols);
  }
  auto tablePROJ = std::make_unique<nanoaod::FlatTable>(transP.size(), name_+"PROJ", false);
  tablePROJ->addColumn<float>("value", transP, "transfer matrix", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tablePROJ), name_+"PROJ");

  auto tableRES3 = std::make_unique<nanoaod::FlatTable>(trans3.size(), name_+"RES3", false);
  tableRES3->addColumn<float>("value", trans3, "transfer matrix", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRES3), name_+"RES3");

  auto tableRES4 = std::make_unique<nanoaod::FlatTable>(trans4.size(), name_+"RES4", false);
  tableRES4->addColumn<float>("value", trans4, "transfer matrix", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRES4), name_+"RES4");

  auto tableBK = std::make_unique<nanoaod::FlatTable>(iReco.size(), name_+"BK", false);
  tableBK->addColumn<int>("iReco", iReco, "Reco index", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("iGen", iGen, "Gen index", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableBK), name_+"BK");

  auto tablePROJBK = std::make_unique<nanoaod::FlatTable>(nTransP.size(), name_+"PROJBK", false);
  tablePROJBK->addColumn<int>("nTrans", nTransP, "Number of elements in transfer matrix", nanoaod::FlatTable::IntColumn);
  tablePROJBK->addColumn<int>("nReco", nRecoP, "Number elements in transfer matrix reco axis", nanoaod::FlatTable::IntColumn);
  tablePROJBK->addColumn<int>("nGen", nGenP, "Number elements in transfer matrix gen axis", nanoaod::FlatTable::IntColumn);
  tablePROJBK->addColumn<int>("order", order, "Order label", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tablePROJBK), name_+"PROJBK");

  auto tableRES3BK = std::make_unique<nanoaod::FlatTable>(nTrans3.size(), name_+"RES3BK", false);
  tableRES3BK->addColumn<int>("nTrans", nTrans3, "Number of elements in transfer matrix", nanoaod::FlatTable::IntColumn);
  tableRES3BK->addColumn<int>("nReco", nReco3, "Number elements in transfer matrix reco axis", nanoaod::FlatTable::IntColumn);
  tableRES3BK->addColumn<int>("nGen", nGen3, "Number elements in transfer matrix gen axis", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableRES3BK), name_+"RES3BK");

  auto tableRES4BK = std::make_unique<nanoaod::FlatTable>(nTrans4.size(), name_+"RES4BK", false);
  tableRES4BK->addColumn<int>("nTrans", nTrans3, "Number of elements in transfer matrix", nanoaod::FlatTable::IntColumn);
  tableRES4BK->addColumn<int>("nReco", nReco4, "Number elements in transfer matrix reco axis", nanoaod::FlatTable::IntColumn);
  tableRES4BK->addColumn<int>("nGen", nGen4, "Number elements in transfer matrix gen axis", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableRES4BK), name_+"RES4BK");
}

DEFINE_FWK_MODULE(EECTransferTableProducer);
