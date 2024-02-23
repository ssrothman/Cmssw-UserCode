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
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "SRothman/SimonTools/src/jets.h"
#include "SRothman/SimonTools/src/util.h"
#include "SRothman/SimonTools/src/etaPhiCoords.h"

#include "SRothman/CustomJets/plugins/AddParticle.h"
#include "SRothman/SimonTools/src/isID.h"
#include "SRothman/SimonTools/src/particleThresholds.h"
#include "SRothman/SimonTools/src/vtxCuts.h"

#include <iostream>
#include <memory>
#include <vector>

class FixedConeJetProducer : public edm::stream::EDProducer<> {
public:
    explicit FixedConeJetProducer(const edm::ParameterSet&);
    ~FixedConeJetProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    
    struct particleThresholds thresholds_;
    struct vtxCuts vtxcuts_;

    bool onlyCharged_;

    unsigned int maxNumPart_, minNumPart_;
    
    bool applyPuppi_;

    edm::InputTag evtSelSrc_;
    edm::EDGetTokenT<bool> evtSelToken_;
    bool doEvtSel_;

    edm::InputTag coordSrc_;
    edm::EDGetTokenT<etaPhiCoords> coordToken_;

    edm::InputTag partSrc_;
    edm::EDGetTokenT<edm::View<reco::Candidate>> partToken_;

    double conesize_;

    int verbose_;
};

FixedConeJetProducer::FixedConeJetProducer(const edm::ParameterSet& conf)
        : thresholds_(conf.getParameter<edm::ParameterSet>("thresholds")),
          vtxcuts_(conf.getParameter<edm::ParameterSet>("vtxCuts")),
          onlyCharged_(conf.getParameter<bool>("onlyCharged")),
          maxNumPart_(conf.getParameter<unsigned>("maxNumPart")),
          minNumPart_(conf.getParameter<unsigned>("minNumPart")),
          evtSelSrc_(conf.getParameter<edm::InputTag>("eventSelection")),
          evtSelToken_(consumes<bool>(evtSelSrc_)),
          doEvtSel_(conf.getParameter<bool>("doEventSelection")),
          coordSrc_(conf.getParameter<edm::InputTag>("coords")),
          coordToken_(consumes<etaPhiCoords>(coordSrc_)),
          partSrc_(conf.getParameter<edm::InputTag>("particles")),
          partToken_(consumes<edm::View<reco::Candidate>>(partSrc_)),
          conesize_(conf.getParameter<double>("conesize")),
          verbose_(conf.getParameter<int>("verbose")){
    produces<std::vector<jet>>();
}

void FixedConeJetProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  edm::ParameterSetDescription thresholdPset;
  particleThresholds::fillPSetDescription(thresholdPset);
  desc.add<edm::ParameterSetDescription>(
            "thresholds", thresholdPset);

  desc.add<bool>("onlyCharged");

  edm::ParameterSetDescription vtxCutsPset;
  vtxCuts::fillPSetDescription(vtxCutsPset);
  desc.add<edm::ParameterSetDescription>(
            "vtxCuts", vtxCutsPset);

  desc.add<unsigned>("maxNumPart");
  desc.add<unsigned>("minNumPart");

  desc.add<edm::InputTag>("eventSelection");
  desc.add<bool>("doEventSelection");

  desc.add<edm::InputTag>("coords");

  desc.add<edm::InputTag>("particles");

  desc.add<int>("verbose");

  desc.add<bool>("applyPuppi");

  desc.add<double>("conesize");

  descriptions.addWithDefaultLabel(desc);
}

void FixedConeJetProducer::produce(edm::Event& evt, 
                                   const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of FixedConeJetProducer::produce()\n");
    }

    edm::Handle<etaPhiCoords> coords;
    evt.getByToken(coordToken_, coords);

    if(verbose_){
        printf("got coords\n");
    }

    edm::Handle<edm::View<reco::Candidate>> parts;
    evt.getByToken(partToken_, parts);

    if(verbose_){
        printf("got particles\n");
    }

    auto result = std::make_unique<std::vector<jet>>();

    if(doEvtSel_){
        edm::Handle<bool> evtSel;
        evt.getByToken(evtSelToken_, evtSel);
        if(!*evtSel){
            evt.put(std::move(result));
            return;
        }
    }

    if(verbose_){
        printf("passed event selection\n");
    }

    jet ans;
    ans.eta = coords->eta;
    ans.phi = coords->phi;
    ans.iJet = 9999;
    ans.jecfactor = 9999;
    ans.pt = 0;

    //printf("making FixedCone jet with eta: %f, phi: %f\n", ans.eta, ans.phi);
    for(unsigned iPart = 0; iPart < parts->size(); iPart++){
        auto part = parts->ptrAt(iPart);

        if(verbose_>1){
            printf("part: (%f, %f, %f, %f)\n", part->pt(),
                                           part->eta(), 
                                           part->phi(),
                                           part->mass());
        }
        const auto* partptr = dynamic_cast<const pat::PackedCandidate*>(part.get());
        const auto* genptr = dynamic_cast<const pat::PackedGenParticle*>(part.get());
        
        if(reco::deltaR(part->eta(), part->phi(), 
                    coords->eta, coords->phi) > conesize_){
            continue;
        }

        if(partptr){
            addParticle(partptr, ans, 9999,
                        applyPuppi_, false, 
                        onlyCharged_,
                        9999, thresholds_,
                        vtxcuts_,
                        maxNumPart_);
       } else if(genptr){
            addParticle(genptr, ans, 9999, 
                        applyPuppi_, false,
                        onlyCharged_,
                        9999, thresholds_,
                        vtxcuts_,
                        maxNumPart_);
       } else {
            throw std::runtime_error("constituent is not a PackedCandidate or PackedGenCandidate");
        }
    } // end for part

    ans.pt = ans.rawpt;

    if(ans.nPart >= minNumPart_){
        result->push_back(std::move(ans));
    }

    if(verbose_){
        printf("pushed back\n");
    }

    evt.put(std::move(result));
    if(verbose_){
        printf("put into event\n");
    }
}  // end produce()

DEFINE_FWK_MODULE(FixedConeJetProducer);
