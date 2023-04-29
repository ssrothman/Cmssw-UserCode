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
#include "DataFormats/Math/interface/deltaR.h"

#include "SRothman/Matching/src/toyjets/common.h"

#include "SRothman/CustomJets/plugins/ParticleUncertainty.h"

#include <iostream>
#include <memory>
#include <vector>

static reco::Muon::Selector muIDFromStr(std::string x){
    if(x == "loose") return reco::Muon::CutBasedIdLoose;
    else if(x == "medium") return reco::Muon::CutBasedIdMedium;
    else if(x == "tight") return reco::Muon::CutBasedIdTight;
    else {
        throw cms::Exception("Unsupported Muon ID type. Pleasre implement your ID in muIDFromStr()");
    }
}

static reco::Muon::Selector muIsoFromStr(std::string x){
    if(x == "loose") return reco::Muon::PFIsoLoose;
    else if(x == "medium") return reco::Muon::PFIsoMedium;
    else if(x == "tight") return reco::Muon::PFIsoTight;
    else {
        throw cms::Exception("Unsupported Muon Isolation type. Pleasre implement your ID in muIsoFromStr()");
    }
}

class ZMuMuEventSelectionProducer : public edm::stream::EDProducer<> {
public:
    explicit ZMuMuEventSelectionProducer(const edm::ParameterSet&);
    ~ZMuMuEventSelectionProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    double minMuPt_;
    double maxMuEta_;

    unsigned long ID_;
    unsigned long Iso_;

    double minZmass_;
    double maxZmass_;

    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<pat::Muon>> srcToken_;

    int verbose_;
};


ZMuMuEventSelectionProducer::ZMuMuEventSelectionProducer(const edm::ParameterSet& conf)
        : minMuPt_(conf.getParameter<double>("minMuPt")),
          maxMuEta_(conf.getParameter<double>("maxMuEta")),
          ID_(muIDFromStr(conf.getParameter<std::string>("ID"))),
          Iso_(muIsoFromStr(conf.getParameter<std::string>("Iso"))),
          minZmass_(conf.getParameter<double>("minZmass")),
          maxZmass_(conf.getParameter<double>("maxZmass")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<pat::Muon>>(src_)),
          verbose_(conf.getParameter<int>("verbose")){
    produces<bool>();
}


void ZMuMuEventSelectionProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<double>("minMuPt");
  desc.add<double>("maxMuEta");
  desc.add<double>("minZmass");
  desc.add<double>("maxZmass");
  desc.add<std::string>("ID");
  desc.add<std::string>("Iso");
  desc.add<edm::InputTag>("src");
  desc.add<int>("verbose");
  descriptions.addWithDefaultLabel(desc);
}


void ZMuMuEventSelectionProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<pat::Muon>> muons;
  evt.getByToken(srcToken_, muons);

  auto result = std::make_unique<bool>(false);

  if(muons->size() >= 2) {
    std::vector<unsigned> passedMuons;
    for(unsigned i=0; i<muons->size(); ++i){
        if(muons->at(i).pt() < minMuPt_ || std::abs(muons->at(i).eta()) > maxMuEta_){
            continue; //failed kinematic cuts
        }
        if(!muons->at(i).passed(ID_) || !muons->at(i).passed(Iso_)){
            continue; //failed ID or isolation
        }
        passedMuons.push_back(i);
    }
    if(passedMuons.size() >= 2){
        const auto& mu1 = muons->at(passedMuons[0]);
        const auto& mu2 = muons->at(passedMuons[1]);
        if(mu1.charge() * mu2.charge() < 0){//opposite sign
            const auto& z = mu1.p4() + mu2.p4();
            printf("Z mass: %f\n", z.mass());
            if(z.mass() > minZmass_ && z.mass() < maxZmass_){
                *result = true;
            }
        }
    }
  }

  evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(ZMuMuEventSelectionProducer);
