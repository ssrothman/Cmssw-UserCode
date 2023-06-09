#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"


#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "SRothman/Analysis/src/RoccoR.h"

class RoccoRValueMapProducer : public edm::stream::EDProducer<>{
public:
    explicit RoccoRValueMapProducer(const edm::ParameterSet&);
    ~RoccoRValueMapProducer() override { }

private:
    void produce(edm::Event&, const edm::EventSetup&) override;

    bool isMC_;

    edm::InputTag muonTag_;
    edm::EDGetTokenT<edm::View<reco::Muon> > muonToken_;

    edm::InputTag genPartTag_;
    edm::EDGetTokenT<edm::View<reco::Candidate> > genPartToken_;

    RoccoR rc_;
};

RoccoRValueMapProducer::RoccoRValueMapProducer(const edm::ParameterSet& iConfig):
    isMC_(iConfig.getParameter<bool>("isMC")),
    muonTag_(iConfig.getParameter<edm::InputTag>("src")),
    muonToken_(consumes<edm::View<reco::Muon> >(muonTag_)),
    genPartTag_(iConfig.getParameter<edm::InputTag>("genParticles")),
    rc_(iConfig.getParameter<edm::FileInPath>("dataFile").fullPath())
{
    if(isMC_){
        genPartToken_ = consumes<edm::View<reco::Candidate> >(genPartTag_);
    }
    produces<edm::ValueMap<float> >();
}

void RoccoRValueMapProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
    edm::Handle<edm::View<reco::Muon> > muons;
    iEvent.getByToken(muonToken_, muons);


    edm::Handle<edm::View<reco::Candidate> > genParts;
    if(isMC_){
        iEvent.getByToken(genPartToken_, genParts);
    }

    std::vector<float> values;
    values.reserve(muons->size());

    std::vector<bool> taken;
    if(isMC_){
            taken.resize(genParts->size(), false);
    }

    for(size_t i = 0; i < muons->size(); ++i){
        const auto& muon = muons->at(i);
        if(isMC_){
            int imatch = -1;
            for(size_t j = 0; j<genParts->size(); ++j){
                const auto& genPart = genParts->at(j);
                if(taken[j]) continue;
                if(genPart.pdgId() != 13 && genPart.pdgId() != -13) continue;
                if(reco::deltaR(muon, genPart) < 0.1){
                    imatch = j;
                    taken[j] = true;
                    break;
                }
            }
            if(imatch >=0){
                values.push_back(rc_.kSpreadMC(muon.charge(), muon.pt(), muon.eta(), muon.phi(), genParts->at(imatch).pt()));
            } else {
                int ntrk = 0;
                if(muon.innerTrack().isNonnull()) {
                    ntrk = muon.innerTrack()->hitPattern().trackerLayersWithMeasurement();
                }
                values.push_back(rc_.kSmearMC(muon.charge(), muon.pt(), muon.eta(), muon.phi(), ntrk,
                                 (float)(rand()) / (float)(RAND_MAX)));
            }
        }
        else{
            values.push_back(rc_.kScaleDT(muon.charge(), muon.pt(), muon.eta(), muon.phi()));
        }
    }

    std::unique_ptr<edm::ValueMap<float> > out = std::make_unique<edm::ValueMap<float> >();
    edm::ValueMap<float>::Filler filler(*out);
    filler.insert(muons, values.begin(), values.end());
    filler.fill();

    iEvent.put(std::move(out));
}

DEFINE_FWK_MODULE(RoccoRValueMapProducer);
