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

class CorrectedMuonProducer : public edm::stream::EDProducer<>{
public:
    explicit CorrectedMuonProducer(const edm::ParameterSet&);
    ~CorrectedMuonProducer() override { }

    void produce(edm::Event&, const edm::EventSetup&) override;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    edm::InputTag muonTag_;
    edm::EDGetTokenT<edm::View<reco::Muon> > muonToken_;

    edm::InputTag RoccoRTag_;
    edm::EDGetTokenT<edm::ValueMap<float> > RoccoRToken_;

    int verbose_;
};

CorrectedMuonProducer::CorrectedMuonProducer(const edm::ParameterSet& iConfig):
    muonTag_(iConfig.getParameter<edm::InputTag>("src")),
    muonToken_(consumes<edm::View<reco::Muon> >(muonTag_)),
    RoccoRTag_(iConfig.getParameter<edm::InputTag>("RoccoR")),
    RoccoRToken_(consumes<edm::ValueMap<float> >(RoccoRTag_)),
    verbose_(iConfig.getParameter<int>("verbose"))
{
    produces<std::vector<reco::Muon>>();
}

void CorrectedMuonProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src");
    desc.add<edm::InputTag>("RoccoR");
    desc.add<int>("verbose");
    descriptions.addWithDefaultLabel(desc);
}

void CorrectedMuonProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup){
    edm::Handle<edm::View<reco::Muon> > muons;
    iEvent.getByToken(muonToken_, muons);

    edm::Handle<edm::ValueMap<float> > RoccoR;
    iEvent.getByToken(RoccoRToken_, RoccoR);

    auto result = std::make_unique<std::vector<reco::Muon>>();

    for(size_t i = 0; i < muons->size(); i++){
        reco::Muon muon = muons->at(i);
        float rc = (*RoccoR)[muons->refAt(i)];
        if(verbose_ > 0){
            printf("Muon %lu: pT = %f, eta = %f, phi = %f, q = %d, rc = %f\n", i, muon.pt(), muon.eta(), muon.phi(), muon.charge(), rc);
        }
        muon.setP4(muon.p4() * rc);
        result->push_back(muon);
    }

    iEvent.put(std::move(result));
}

DEFINE_FWK_MODULE(CorrectedMuonProducer);
