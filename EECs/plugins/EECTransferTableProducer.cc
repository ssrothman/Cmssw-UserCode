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

#include "SRothman/DataFormats/interface/EEC.h"

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cstdarg>

#define MAX_CONSTITUENTS 256

#define VERBOSE 

// requires at least C++11
//from https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/49812018#49812018
static const std::string vformat(const char * const zcFormat, ...) {

  // initialize use of the variable argument array
  va_list vaArgs;
  va_start(vaArgs, zcFormat);

  // reliably acquire the size
  // from a copy of the variable argument array
  // and a functionally reliable call to mock the formatting
  va_list vaArgsCopy;
  va_copy(vaArgsCopy, vaArgs);
  const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
  va_end(vaArgsCopy);

  // return a formatted string without risking memory mismanagement
  // and without assuming any compiler or platform specific behavior
  std::vector<char> zc(iLen + 1);
  std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
  va_end(vaArgs);
  return std::string(zc.data(), iLen); 
}

class EECTransferTableProducer : public edm::stream::EDProducer<> {
public:
  explicit EECTransferTableProducer(const edm::ParameterSet&);
  ~EECTransferTableProducer() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  std::string name_;

  edm::InputTag transferTag_;
  edm::EDGetTokenT<EECTransferCollection> transferToken_;
};

EECTransferTableProducer::EECTransferTableProducer(const edm::ParameterSet& conf)
    : 
      name_(conf.getParameter<std::string>("name")),
      transferTag_(conf.getParameter<edm::InputTag>("transfer")),
      transferToken_(consumes<EECTransferCollection>(transferTag_)){
  produces<nanoaod::FlatTable>(name_ + "BK");
  produces<nanoaod::FlatTable>(name_ + "GDR");
  produces<nanoaod::FlatTable>(name_ + "RDR");
  produces<nanoaod::FlatTable>(name_ + "MAT");
}

void EECTransferTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("transfer");
  descriptions.addWithDefaultLabel(desc);
}

void EECTransferTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<EECTransferCollection> transfers;
  evt.getByToken(transferToken_, transfers);

  //actually four tables
  
  //bookkeeping table
  auto recoJetIdx = std::make_unique<std::vector<int>>();
  auto genJetIdx = std::make_unique<std::vector<int>>();
  auto nGenDR = std::make_unique<std::vector<int>>();
  auto nRecoDR = std::make_unique<std::vector<int>>();

  //three different shape other tables
  auto genDR = std::make_unique<std::vector<float>>();
  auto recoDR = std::make_unique<std::vector<float>>();
  auto matrix = std::make_unique<std::vector<float>>();

  for(const auto& transfer: *transfers){
    //bookkeeping
    recoJetIdx->emplace_back(transfer.iJetReco);
    genJetIdx->emplace_back(transfer.iJetGen);
    nGenDR->emplace_back(transfer.dRgen->size());
    nRecoDR->emplace_back(transfer.dRreco->size());

    //genDR table
    genDR->insert(genDR->end(), transfer.dRgen->begin(), transfer.dRgen->end());

    //recoDR table
    recoDR->insert(recoDR->end(), transfer.dRreco->begin(), transfer.dRreco->end());

    //actual transfer matrix
    for(size_t iReco=0; iReco<transfer.dRreco->size(); ++iReco){
      for(size_t iGen=0; iGen<transfer.dRgen->size(); ++ iGen){
        matrix->emplace_back((*transfer.matrix)(iReco, iGen));
      }
    }
  }

  auto tableBK = std::make_unique<nanoaod::FlatTable>(nGenDR->size(), 
                                                      name_ + "BK", 
                                                      false);
  tableBK->addColumn<int>("recoJetIdx", *recoJetIdx, "Index of corresponding reco jet", 
                          nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("genJetIdx", *genJetIdx, "Index of corresponding gen jet", 
                          nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nGenDR", *nGenDR, "Number of dR values in gen", 
                          nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nRecoDR", *nRecoDR, "Number of dR values in reco", 
                          nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableBK), name_ + "BK");
  
  auto tableGDR = std::make_unique<nanoaod::FlatTable>(genDR->size(), 
                                                       name_ + "GDR", 
                                                       false);
  tableGDR->addColumn<float>("genDR", *genDR, "Gen-level dR values", 
                              nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableGDR), name_ + "GDR");

  auto tableRDR = std::make_unique<nanoaod::FlatTable>(recoDR->size(), 
                                                       name_ + "RDR", 
                                                       false);
  tableRDR->addColumn<float>("recoDR", *recoDR, "Reco-level dR values", 
                              nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableRDR), name_ + "RDR");

  auto tableMAT = std::make_unique<nanoaod::FlatTable>(matrix->size(), 
                                                       name_ + "MAT", 
                                                       false);
  tableMAT->addColumn<float>("matrix", *matrix, "Transfer matrix", 
                              nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableMAT), name_ + "MAT");
}  // end produce()

DEFINE_FWK_MODULE(EECTransferTableProducer);
