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

#include "SRothman/SimonTools/src/jet.h"
#include "SRothman/SimonTools/src/util.h"

#include "SRothman/SimonTools/src/isID.h"
#include "SRothman/SimonTools/src/particleSelector.h"

#include <iostream>
#include <memory>
#include <vector>

template <typename T>
class SimonJetProducerT : public edm::stream::EDProducer<> {
public:
    explicit SimonJetProducerT(const edm::ParameterSet&);
    ~SimonJetProducerT() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    simon::particleSelector selector_;

    edm::InputTag jetSrc_;
    edm::EDGetTokenT<edm::View<T>> jetSrcToken_;

    edm::InputTag pfCand_;
    edm::EDGetTokenT<edm::View<pat::PackedCandidate>> pfCandToken_;

    edm::InputTag CHSsrc_;
    edm::EDGetTokenT<edm::View<T>> CHSsrcToken_;
    bool addCHSindex_;
    double CHSmatchDR_;

    int verbose_;
};

template <typename T>
SimonJetProducerT<T>::SimonJetProducerT(const edm::ParameterSet& conf) :
          selector_(conf.getParameter<edm::ParameterSet>("selector")),
          jetSrc_(conf.getParameter<edm::InputTag>("jetSrc")),
          jetSrcToken_(consumes<edm::View<T>>(jetSrc_)),
	  pfCand_(conf.getParameter<edm::InputTag>("pfCandidates")),
          pfCandToken_(consumes<edm::View<pat::PackedCandidate>>(pfCand_)),
          CHSsrc_(conf.getParameter<edm::InputTag>("CHSsrc")),
          CHSsrcToken_(consumes<edm::View<T>>(CHSsrc_)),
          addCHSindex_(conf.getParameter<bool>("addCHSindex")),
          CHSmatchDR_(conf.getParameter<double>("CHSmatchDR")),
          verbose_(conf.getParameter<int>("verbose")){
    produces<std::vector<simon::jet>>();
}

template <typename T>
void SimonJetProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  edm::ParameterSetDescription selectorDesc;
  simon::particleSelector::fillPSetDescription(selectorDesc);
  desc.add<edm::ParameterSetDescription>("selector", selectorDesc);

  desc.add<edm::InputTag>("jetSrc");
  desc.add<edm::InputTag>("CHSsrc");
  desc.add<edm::InputTag>("pfCandidates");
  desc.add<bool>("addCHSindex");
  desc.add<double>("CHSmatchDR");

  desc.add<int>("verbose");

  descriptions.addWithDefaultLabel(desc);
}

template <typename T>
void SimonJetProducerT<T>::produce(edm::Event& evt, 
                                   const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of SimonJetProducerT<T>::produce()\n");
    }
    edm::Handle<edm::View<T>> jets;
    evt.getByToken(jetSrcToken_, jets);

    edm::Handle<edm::View<pat::PackedCandidate>> candidates;
    evt.getByToken(pfCandToken_, candidates);

    edm::Handle<edm::View<T>> CHSjets;
    if(addCHSindex_){
        evt.getByToken(CHSsrcToken_, CHSjets);
    }

    std::cout << "SimonJetProducer::produce called" << std::endl;    
    auto result = std::make_unique<std::vector<simon::jet>>();
    simon::jet evt_jet;
        
    size_t mu1 = 999999;
    size_t mu2 = 999999;
    float pt1 = -1.0;
    float pt2 = -1.0;
    float zpt = 0.0;

    for (size_t i = 0; i < candidates->size(); ++i) {
       const auto& cand = candidates->at(i);
       if (std::abs(cand.pdgId()) != 13) continue;

       float pt = cand.pt();
       if (pt > pt1) {
           pt2 = pt1;
           mu2 = mu1;
           pt1 = pt;
           mu1 = i;
       } else if (pt > pt2) {
           pt2 = pt;
           mu2 = i;
       }
    }

    if (mu1 != 999999) zpt += pt1;
    if (mu2 != 999999) zpt += pt2;

    evt_jet.pt = zpt;
    evt_jet.eta = 0;
    evt_jet.phi = 0;
    evt_jet.mass = 1;
    evt_jet.iJet = 0;

    std::vector<edm::Ptr<pat::PackedCandidate>> chargedPtrs;
    chargedPtrs.reserve(candidates->size());
    for (size_t i = 0; i < candidates->size(); ++i) {
        const auto& cand = candidates->at(i);
        if (cand.charge() == 0) continue;
	if (i == mu1 || i == mu2) continue;
        chargedPtrs.emplace_back(candidates->ptrAt(i));
    }

    selector_.buildJet(chargedPtrs, evt_jet);
    printf("###################################");
    printf("evt_jet is built\n");
    printf("###################################");
    result->clear();
    result->push_back(std::move(evt_jet));
    printf("result has %zu entries\n", result->size());
    evt.put(std::move(result));
    if(verbose_){
        printf("put into event\n");
    }
}  // end produce()

typedef SimonJetProducerT<pat::Jet> PatSimonJetProducer;
typedef SimonJetProducerT<reco::GenJet> GenSimonJetProducer;

DEFINE_FWK_MODULE(PatSimonJetProducer);
DEFINE_FWK_MODULE(GenSimonJetProducer);
