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
#include "SRothman/SimonTools/src/isID.h"

#include <iostream>
#include <memory>
#include <vector>

#include <Eigen/Dense>

class SimonJetTableProducer : public edm::stream::EDProducer<> {
public:
    explicit SimonJetTableProducer(const edm::ParameterSet&);
    ~SimonJetTableProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    std::string name_;
    
    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<simon::jet>> srcToken_;

    std::vector<std::string> extraFloatNames_;
    std::vector<edm::InputTag> extraFloatTags_;
    std::vector<edm::EDGetTokenT<edm::ValueMap<float>>> extraFloatTokens_;

    int verbose_;
};

SimonJetTableProducer::SimonJetTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<simon::jet>>(src_)),
          extraFloatNames_(conf.getParameter<std::vector<std::string>>("extraFloatNames")),
          extraFloatTags_(conf.getParameter<std::vector<edm::InputTag>>("extraFloats")),
          verbose_(conf.getParameter<int>("verbose")){

    if(extraFloatNames_.size() != extraFloatTags_.size()){
        throw cms::Exception("Configuration") << "extraFloatNames and extraFloats must have the same size";
    }

    for(const auto& tag : extraFloatTags_){
        extraFloatTokens_.push_back(consumes<edm::ValueMap<float>>(tag));
    }

    produces<nanoaod::FlatTable>(name_);
    produces<nanoaod::FlatTable>(name_+"CHS");
    produces<nanoaod::FlatTable>(name_+"BK");
}

void SimonJetTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<int>("verbose");
  desc.add<edm::InputTag>("src");
  desc.add<std::vector<std::string>>("extraFloatNames", std::vector<std::string>())->setComment("Names for ValueMap<float> columns in BK table");
  desc.add<std::vector<edm::InputTag>>("extraFloats", std::vector<edm::InputTag>())->setComment("InputTags for ValueMap<float> to add to BK table");
  descriptions.addWithDefaultLabel(desc);
}

void SimonJetTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of SimonJetTableProducer::produce()\n");
    }
  edm::Handle<edm::View<simon::jet>> jets;
  evt.getByToken(srcToken_, jets);

  std::vector<float>  partPt;
  std::vector<float> partEta;
  std::vector<float> partPhi;
  std::vector<int> pdgid;
  std::vector<int> charge;
  //extras
  std::vector<float> vtx_x, vtx_y, vtx_z;
  std::vector<float> dxy, dz;
  std::vector<int> fromPV;
  std::vector<float> puppiWeight;

  std::vector<float> pt;
  std::vector<float> rawPt;
  std::vector<float> eta;
  std::vector<float> phi;
  std::vector<int> iJet;
  std::vector<float> mass;
  std::vector<int> nPart;
  std::vector<float> jecfactor;

  std::vector<int> iCHS;
  std::vector<int> nCHS;

  // Get ValueMaps
  std::vector<edm::Handle<edm::ValueMap<float>>> extrafloats;
  std::vector<std::vector<float>> extraFloatData(extraFloatTokens_.size());
  for(size_t i = 0; i < extraFloatTokens_.size(); ++i){
      edm::Handle<edm::ValueMap<float>> handle;
      evt.getByToken(extraFloatTokens_[i], handle);
      extrafloats.push_back(handle);
  }

  unsigned iJ=0;
  for(const auto& j : *jets){
      pt.push_back(j.pt);
      rawPt.push_back(j.rawpt);
      jecfactor.push_back(j.jecfactor);
      eta.push_back(j.eta);
      phi.push_back(j.phi);
      iJet.push_back(j.iJet);
      mass.push_back(j.mass);
      nPart.push_back(j.nPart);

      iCHS.insert(iCHS.end(), j.iCHS.begin(), j.iCHS.end());
      nCHS.push_back(j.iCHS.size());

      // Get ValueMap values for this jet
      for(size_t i = 0; i < extrafloats.size(); ++i){
          extraFloatData[i].push_back((*extrafloats[i])[jets->refAt(iJ)]);
      }

      for(const auto& p : j.particles){
          partPt.push_back(p.pt);
          partEta.push_back(p.eta);
          partPhi.push_back(p.phi);
          pdgid.push_back(p.pdgid);
          charge.push_back(p.charge);
          vtx_x.push_back(p.vtx_x);
          vtx_y.push_back(p.vtx_y);
          vtx_z.push_back(p.vtx_z);
          dxy.push_back(p.dxy);
          dz.push_back(p.dz);
          fromPV.push_back(p.fromPV);
          puppiWeight.push_back(p.puppiweight);
      }
      ++iJ;
  }
    if(verbose_){
        printf("filled vectors\n");
    }

  auto table = std::make_unique<nanoaod::FlatTable>(partPt.size(), name_, false);
  table->addColumn<float>("pt", partPt, "particle pt", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("eta", partEta, "particle eta", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("phi", partPhi, "particle phi", nanoaod::FlatTable::FloatColumn);
  table->addColumn<int>("pdgid", pdgid, "particle pdgid", nanoaod::FlatTable::IntColumn);
  table->addColumn<int>("charge", charge, "particle charge", nanoaod::FlatTable::IntColumn);
  table->addColumn<float>("vtx_x", vtx_x, "vertex x coord", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("vtx_y", vtx_y, "vertex y coord", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("vtx_z", vtx_z, "vertex z coord", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("dxy", dxy, "dxy", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("dz", dz, "dz", nanoaod::FlatTable::FloatColumn);
  table->addColumn<int>("fromPV", fromPV, "from PV enum", nanoaod::FlatTable::IntColumn);
  table->addColumn<float>("puppiWeight", puppiWeight, "Puppi weight", nanoaod::FlatTable::FloatColumn);

  evt.put(std::move(table), name_);
  if(verbose_){
    printf("made table with %lu elements\n", partPt.size());
  }

  auto tableCHS = std::make_unique<nanoaod::FlatTable>(iCHS.size(), name_+"CHS", false);
  tableCHS->addColumn<int>("idx", iCHS, "index of matched CHS jet", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableCHS), name_+"CHS");

  auto tableBK = std::make_unique<nanoaod::FlatTable>(pt.size(), name_+"BK", false);
  tableBK->addColumn<float>("jetPt", pt, "jet pt", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<float>("jetRawPt", rawPt, "raw jet pt", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<float>("jecfactor", jecfactor, "JEC factor", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<float>("jetEta", eta, "jet eta", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<float>("jetPhi", phi, "jet phi", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<int>("iJet", iJet, "index in primary jet array", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<float>("jetMass", mass, "jet mass", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<int>("nPart", nPart, "number of particles in jet", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nCHS", nCHS, "number of matched CHS jets", nanoaod::FlatTable::IntColumn);
  
  // Add ValueMap columns
  for(size_t i = 0; i < extraFloatNames_.size(); ++i){
      tableBK->addColumn<float>(extraFloatNames_[i], extraFloatData[i], extraFloatNames_[i], nanoaod::FlatTable::FloatColumn);
  }
  
  evt.put(std::move(tableBK), name_+"BK");
  if(verbose_){
    printf("made tableBK with %lu elements\n", pt.size());
  }
}

DEFINE_FWK_MODULE(SimonJetTableProducer);
