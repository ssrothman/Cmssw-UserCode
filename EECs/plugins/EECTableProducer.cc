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
    produces<nanoaod::FlatTable>(name_+"RES3");
    produces<nanoaod::FlatTable>(name_+"RES4");
    produces<nanoaod::FlatTable>(name_+"COVPxP");
    produces<nanoaod::FlatTable>(name_+"COV3x3");
    produces<nanoaod::FlatTable>(name_+"COV3xP");
    produces<nanoaod::FlatTable>(name_+"COV4x4");
    produces<nanoaod::FlatTable>(name_+"COV4x3");
    produces<nanoaod::FlatTable>(name_+"COV4xP");
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
  edm::Handle<edm::View<EECresult>> EECs;
  evt.getByToken(srcToken_, EECs);

  std::vector<float> wts;
  std::vector<float> dRs;

  std::vector<float> res3wts;
  std::vector<float> res3dR1;
  std::vector<float> res3dR2;
  std::vector<float> res3dR3;

  std::vector<float> res4wts;
  std::vector<float> res4dR1;
  std::vector<float> res4dR2;
  std::vector<float> res4dR3;
  std::vector<float> res4dR4;
  std::vector<float> res4dR5;
  std::vector<float> res4dR6;

  std::vector<float> covPxP;
  std::vector<float> cov3x3;
  std::vector<float> cov3xP;
  std::vector<float> cov4x4;
  std::vector<float> cov4x3;
  std::vector<float> cov4xP;

  std::vector<int> offsets;
  std::vector<int> order;

  std::vector<int> iJet;
  std::vector<int> nOrders;
  std::vector<int> nWts;
  std::vector<int> nDR;
  std::vector<int> ncovPxP;
  std::vector<int> ncov3x3;
  std::vector<int> ncov3xP;
  std::vector<int> ncov4x4;
  std::vector<int> ncov4x3;
  std::vector<int> ncov4xP;
  std::vector<int> nRes3;
  std::vector<int> nRes4;

  for(const auto& EEC : *EECs){
      iJet.push_back(EEC.iJet);
      nOrders.push_back(EEC.offsets.size());
      nWts.push_back(EEC.wts.size());
      nDR.push_back(EEC.dRs.size());
      ncovPxP.push_back(EEC.cov.size());
      ncov3x3.push_back(EEC.covRes3Res3.size());
      ncov3xP.push_back(EEC.covRes3Proj.size());
      ncov4x4.push_back(EEC.covRes4Res4.size());
      ncov4x3.push_back(EEC.covRes4Res3.size());
      ncov4xP.push_back(EEC.covRes4Proj.size());
      nRes3.push_back(EEC.res3wts.size());
      nRes4.push_back(EEC.res4wts.size());
      
      offsets.insert(offsets.end(), EEC.offsets.begin(), EEC.offsets.end());
      order.insert(order.end(), EEC.order.begin(), EEC.order.end());

      wts.insert(wts.end(), EEC.wts.begin(), EEC.wts.end());
      dRs.insert(dRs.end(), EEC.dRs.begin(), EEC.dRs.end());

      res3wts.insert(res3wts.end(), EEC.res3wts.begin(), 
                                    EEC.res3wts.end());
      res3dR1.insert(res3dR1.end(), EEC.res3dR1.begin(), 
                                    EEC.res3dR1.end());
      res3dR2.insert(res3dR2.end(), EEC.res3dR2.begin(), 
                                    EEC.res3dR2.end());
      res3dR3.insert(res3dR3.end(), EEC.res3dR3.begin(), 
                                    EEC.res3dR3.end());

      res4wts.insert(res4wts.end(), EEC.res4wts.begin(),
                                    EEC.res4wts.end());
      res4dR1.insert(res4dR1.end(), EEC.res4dR1.begin(),
                                    EEC.res4dR1.end());
      res4dR2.insert(res4dR2.end(), EEC.res4dR2.begin(),
                                    EEC.res4dR2.end());
      res4dR3.insert(res4dR3.end(), EEC.res4dR3.begin(),
                                    EEC.res4dR3.end());
      res4dR4.insert(res4dR4.end(), EEC.res4dR4.begin(),
                                    EEC.res4dR4.end());
      res4dR5.insert(res4dR5.end(), EEC.res4dR5.begin(),
                                    EEC.res4dR5.end());
      res4dR6.insert(res4dR6.end(), EEC.res4dR6.begin(),
                                    EEC.res4dR6.end());

      covPxP.insert(covPxP.end(), EEC.cov.begin(), EEC.cov.end());

      cov3x3.insert(cov3x3.end(), EEC.covRes3Res3.begin(), 
                                  EEC.covRes3Res3.end());
      cov3xP.insert(cov3xP.end(), EEC.covRes3Proj.begin(), 
                                  EEC.covRes3Proj.end());

      cov4x4.insert(cov4x4.end(), EEC.covRes4Res4.begin(), 
                                  EEC.covRes4Res4.end());
      cov4x3.insert(cov4x3.end(), EEC.covRes4Res3.begin(), 
                                  EEC.covRes4Res3.end());
      cov4xP.insert(cov4xP.end(), EEC.covRes4Proj.begin(), 
                                  EEC.covRes4Proj.end());
  }

  auto tableWTS = std::make_unique<nanoaod::FlatTable>(wts.size(), name_+"WTS", false);
  tableWTS->addColumn<float>("value", wts, "EEC weights", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableWTS), name_+"WTS");

  auto tableDRs = std::make_unique<nanoaod::FlatTable>(dRs.size(), name_+"DRS", false);
  tableDRs->addColumn<float>("value", dRs, "EEC dRs", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableDRs), name_+"DRS");

  auto tableRES3 = std::make_unique<nanoaod::FlatTable>(res3wts.size(), name_+"RES3", false);
  tableRES3->addColumn<float>("wts", res3wts, "fully resolved weights", nanoaod::FlatTable::FloatColumn);
  tableRES3->addColumn<float>("dR1", res3dR1, "fully resolved largest dR", nanoaod::FlatTable::FloatColumn);
  tableRES3->addColumn<float>("dR2", res3dR2, "fully resolved second-largest dR", nanoaod::FlatTable::FloatColumn);
  tableRES3->addColumn<float>("dR3", res3dR3, "fully resolved third-largest dR", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRES3), name_+"RES3");

  auto tableRES4 = std::make_unique<nanoaod::FlatTable>(res4wts.size(), name_+"RES4", false);
  tableRES4->addColumn<float>("wts", res4wts, "fully resolved weights", nanoaod::FlatTable::FloatColumn);
  tableRES4->addColumn<float>("dR1", res4dR1, "fully resolved largest dR", nanoaod::FlatTable::FloatColumn);
  tableRES4->addColumn<float>("dR2", res4dR2, "fully resolved second-largest dR", nanoaod::FlatTable::FloatColumn);
  tableRES4->addColumn<float>("dR3", res4dR3, "fully resolved third-largest dR", nanoaod::FlatTable::FloatColumn);
  tableRES4->addColumn<float>("dR4", res4dR4, "fully resolved fourth-largest dR", nanoaod::FlatTable::FloatColumn);
  tableRES4->addColumn<float>("dR5", res4dR5, "fully resolved fifth-largest dR", nanoaod::FlatTable::FloatColumn);
  tableRES4->addColumn<float>("dR6", res4dR6, "fully resolved sixth-largest dR", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRES4), name_+"RES4");

  auto tableCOVPxP = std::make_unique<nanoaod::FlatTable>(covPxP.size(), name_+"COVPxP", false);
  tableCOVPxP->addColumn<float>("value", covPxP, "EEC covariance matrix", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableCOVPxP), name_+"COVPxP");

  auto tableCOV3x3 = std::make_unique<nanoaod::FlatTable>(cov3x3.size(), name_+"COV3x3", false);
  tableCOV3x3->addColumn<float>("value", cov3x3, "EEC covariance matrix", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableCOV3x3), name_+"COV3x3");

  auto tableCOV3xP = std::make_unique<nanoaod::FlatTable>(cov3xP.size(), name_+"COV3xP", false);
  tableCOV3xP->addColumn<float>("value", cov3xP, "EEC covariance matrix", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableCOV3xP), name_+"COV3xP");

  auto tableCOV4x4 = std::make_unique<nanoaod::FlatTable>(cov4x4.size(), name_+"COV4x4", false);
  tableCOV4x4->addColumn<float>("value", cov4x4, "EEC covariance matrix", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableCOV4x4), name_+"COV4x4");

  auto tableCOV4x3 = std::make_unique<nanoaod::FlatTable>(cov4x3.size(), name_+"COV4x3", false);
  tableCOV4x3->addColumn<float>("value", cov4x3, "EEC covariance matrix", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableCOV4x3), name_+"COV4x3");

  auto tableCOV4xP = std::make_unique<nanoaod::FlatTable>(cov4xP.size(), name_+"COV4xP", false);
  tableCOV4xP->addColumn<float>("value", cov4xP, "EEC covariance matrix", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableCOV4xP), name_+"COV4xP");

  auto tableWTSBK = std::make_unique<nanoaod::FlatTable>(offsets.size(), name_+"WTSBK", false);
  tableWTSBK->addColumn<int>("offset", offsets, "offset into wts", nanoaod::FlatTable::IntColumn);
  tableWTSBK->addColumn<int>("order", order, "correlator order", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableWTSBK), name_+"WTSBK");

  auto tableBK = std::make_unique<nanoaod::FlatTable>(iJet.size(), name_+"BK", false);
  tableBK->addColumn<int>("iJet", iJet, "index in jet array", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nOrders", nOrders, "number of EEC orders", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nWts", nWts, "number of EEC weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nDR", nDR, "number of dR points", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("ncovPxP", ncovPxP, "number of covariance matrix elements", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("ncov3x3", ncov3x3, "number of covariance matrix elements", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("ncov3xP", ncov3xP, "number of covariance matrix elements", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("ncov4x4", ncov4x4, "number of covariance matrix elements", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("ncov4x3", ncov4x3, "number of covariance matrix elements", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("ncov4xP", ncov4xP, "number of covariance matrix elements", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nRes3", nRes3, "number of fully resolved 3pt weights", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nRes4", nRes4, "number of fully resolved 4pt weights", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableBK), name_+"BK");
}

DEFINE_FWK_MODULE(EECTableProducer);
