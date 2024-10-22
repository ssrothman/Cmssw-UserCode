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

#include "SRothman/SimonTools/src/jets.h"

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
    double leadMuPt_;
    double subMuPt_;
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
        : leadMuPt_(conf.getParameter<double>("leadMuPt")),
          subMuPt_(conf.getParameter<double>("subMuPt")),
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
  desc.add<double>("leadMuPt");
  desc.add<double>("subMuPt");
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
    if(verbose_){
        printf("Top of ZMuMUEventSelectionProducer::produce()\n");
    }
    edm::Handle<edm::View<pat::Muon>> muons;
    evt.getByToken(srcToken_, muons);

    auto result = std::make_unique<bool>(false);

    if(muons->size() >= 2){
        const auto& mu1 = muons->at(0);
        const auto& mu2 = muons->at(1);
        if(std::abs(mu1.eta()) < maxMuEta_ &&
                std::abs(mu2.eta()) < maxMuEta_ &&
                mu1.passed(ID_) && mu2.passed(ID_) &&
                mu1.passed(Iso_) && mu2.passed(Iso_) &&
                mu1.pt() > leadMuPt_ && 
                mu2.pt() > subMuPt_){

            const auto& z = mu1.p4() + mu2.p4();
            if(verbose_){
                printf("Z mass: %f\n", z.mass());
            }
            if(z.mass() > minZmass_ && z.mass() < maxZmass_){
                *result = true;
            }
        }
    }

    evt.put(std::move(result));
    if(verbose_){
        printf("put into event\n");
    }
}  // end produce()

DEFINE_FWK_MODULE(ZMuMuEventSelectionProducer);
