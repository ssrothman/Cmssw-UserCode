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

#include <iostream>
#include <memory>
#include <vector>

class GenMatchTableProducer : public edm::stream::EDProducer<> {
public:
    explicit GenMatchTableProducer(const edm::ParameterSet&);
    ~GenMatchTableProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    std::string name_;
    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<jetmatch>> srcToken_;

};

GenMatchTableProducer::GenMatchTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<jetmatch>>(src_)){
    produces<nanoaod::FlatTable>(name_);
    produces<nanoaod::FlatTable>(name_+"BK");
}

void GenMatchTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("src");
  descriptions.addWithDefaultLabel(desc);
}

void GenMatchTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<jetmatch>> matches;
  evt.getByToken(srcToken_, matches);

  std::vector<float>  ptrans;

  std::vector<int> iReco;
  std::vector<int> iGen;
  std::vector<int> n_rows;
  std::vector<int> n_cols;

  for(const auto& m : *matches){
      iReco.push_back(m.iReco);
      iGen.push_back(m.iGen);
      n_rows.push_back(m.ptrans.n_rows);
      n_cols.push_back(m.ptrans.n_cols);

      for(unsigned i=0; i<m.ptrans.n_rows; ++i){
          for(unsigned j=0; j<m.ptrans.n_cols; ++j){
              ptrans.push_back(m.ptrans(i,j));
          }
      }
  }

  auto table = std::make_unique<nanoaod::FlatTable>(ptrans.size(), name_, false);
  table->addColumn<float>("matrix", ptrans, "patricle transfer", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(table), name_);

  auto tableBK = std::make_unique<nanoaod::FlatTable>(iReco.size(), name_+"BK", false);
  tableBK->addColumn<int>("iReco", iReco, "index in reco jet array", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("iGen", iReco, "index in gen jet array", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("n_rows", n_rows, "number of rows in transfer matrix", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("n_cols", n_cols, "number of columns in transfer matrix", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableBK), name_+"BK");
}

DEFINE_FWK_MODULE(GenMatchTableProducer);
