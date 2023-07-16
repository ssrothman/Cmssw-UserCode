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

class ResolutionStudyTableProducer : public edm::stream::EDProducer<> {
public:
    explicit ResolutionStudyTableProducer(const edm::ParameterSet&);
    ~ResolutionStudyTableProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    std::string name_;
    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<jetmatch>> srcToken_;
    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;
    edm::InputTag genTag_;
    edm::EDGetTokenT<edm::View<jet>> genToken_;

    int verbose_;

};

ResolutionStudyTableProducer::ResolutionStudyTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<jetmatch>>(src_)),
          recoTag_(conf.getParameter<edm::InputTag>("reco")),
          recoToken_(consumes<edm::View<jet>>(recoTag_)),
          genTag_(conf.getParameter<edm::InputTag>("gen")),
          genToken_(consumes<edm::View<jet>>(genTag_)),
          verbose_(conf.getParameter<int>("verbose")){
    produces<nanoaod::FlatTable>(name_);
    produces<nanoaod::FlatTable>(name_+"BK");
}

void ResolutionStudyTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("src");
  desc.add<edm::InputTag>("reco");
  desc.add<edm::InputTag>("gen");
  desc.add<int>("verbose");
  descriptions.addWithDefaultLabel(desc);
}

void ResolutionStudyTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of ResolutionStudyTableProducer::produce()\n");
    }
  edm::Handle<edm::View<jetmatch>> matches;
  evt.getByToken(srcToken_, matches);
  edm::Handle<edm::View<jet>> recoJets;
  evt.getByToken(recoToken_, recoJets);
  edm::Handle<edm::View<jet>> genJets;
  evt.getByToken(genToken_, genJets);

  std::vector<int> recoCharge, recoPdgId, nMatch;
  std::vector<float> recoPt, predPt, recoEta, predEta, recoPhi, predPhi;

  std::vector<int> jetIdx, nPart;

  for(const auto& m : *matches){
      const auto& jreco = recoJets->at(m.iReco);
      const auto& jgen = genJets->at(m.iGen);

      const arma::vec& ptreco = jreco.ptvec(); 
      const arma::vec& ptgen = jgen.ptvec();
      const arma::vec& etareco = jreco.etavec();
      const arma::vec& etagen = jgen.etavec();
      const arma::vec& phireco = jreco.phivec();
      const arma::vec& phigen = jgen.phivec();

      arma::vec ptpred = m.ptrans * ptgen;
      arma::uvec nonzero = arma::find(ptpred>0);

      ptpred.replace(0, 1);
      arma::vec etapred = m.ptrans * (etagen % ptgen)/ptpred;
      arma::vec phipred = m.ptrans * (phigen % ptgen)/ptpred;
      arma::vec nm = m.ptrans * arma::ones<arma::vec>(ptgen.n_elem);

      for(const auto& iReco : nonzero){
          recoPt.push_back(ptreco(iReco));
          recoEta.push_back(etareco(iReco));
          recoPhi.push_back(phireco(iReco));
          recoPdgId.push_back(jreco.particles[iReco].pdgid);
          recoCharge.push_back(jreco.particles[iReco].charge);
          nMatch.push_back(static_cast<int>(nm(iReco)));

          predPt.push_back(ptpred(iReco));
          predEta.push_back(etapred(iReco));
          predPhi.push_back(phipred(iReco));
      }
      jetIdx.push_back(m.iReco);
      nPart.push_back(nonzero.n_elem);
  }

  auto table = std::make_unique<nanoaod::FlatTable>(recoPt.size(), name_, false);
  table->addColumn<int>("pdgId", recoPdgId, "Reconstructed particle pdgId", nanoaod::FlatTable::IntColumn);
  table->addColumn<int>("charge", recoPdgId, "Reco particle pdgId", nanoaod::FlatTable::IntColumn);
  table->addColumn<int>("nMatch", nMatch, "Number of gen particles matched to this reco one", nanoaod::FlatTable::IntColumn);
  table->addColumn<float>("recoPt", recoPt, "Reco particle pt", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("recoEta", recoEta, "Reco particle eta", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("recoPhi", recoPhi, "Reco particle phi", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("predPt", predPt, "Predicted particle pt", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("predEta", predEta, "Predicted particle eta", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("predPhi", predPhi, "Predicted particle phi", nanoaod::FlatTable::FloatColumn);

  auto tableBK = std::make_unique<nanoaod::FlatTable>(jetIdx.size(), name_+"BK", false);
  tableBK->addColumn<int>("jetIdx", jetIdx, "Reconstructed jet index", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nPart", nPart, "Number of particles in this jet", nanoaod::FlatTable::IntColumn);

  evt.put(std::move(table), name_);
  evt.put(std::move(tableBK), name_+"BK");
  if(verbose_){
      printf("bottom of ResolutionStudyTableProducer::produce()\n");
  }
}

DEFINE_FWK_MODULE(ResolutionStudyTableProducer);
