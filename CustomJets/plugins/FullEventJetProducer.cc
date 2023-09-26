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
#include "SRothman/SimonTools/src/util.h"

#include <iostream>
#include <memory>
#include <vector>

template <typename T>
class FullEventJetProducerT : public edm::stream::EDProducer<> {
public:
    explicit FullEventJetProducerT(const edm::ParameterSet&);
    ~FullEventJetProducerT() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    double minPartPt_;
    double maxEta_;
    unsigned int maxNumPart_;

    int verbose_;

    edm::InputTag partSrc_;
    edm::EDGetTokenT<edm::View<T>> partToken_;

    edm::InputTag evtSelSrc_;
    edm::EDGetTokenT<bool> evtSelToken_;
    bool doEvtSel_;
};

template <typename T>
FullEventJetProducerT<T>::FullEventJetProducerT(const edm::ParameterSet& conf)
        : minPartPt_(conf.getParameter<double>("minPartPt")),
          maxEta_(conf.getParameter<double>("maxEta")),
          maxNumPart_(conf.getParameter<unsigned>("maxNumPart")),
          verbose_(conf.getParameter<int>("verbose")),
          partSrc_(conf.getParameter<edm::InputTag>("partSrc")),
          partToken_(consumes<edm::View<T>>(partSrc_)),
          evtSelSrc_(conf.getParameter<edm::InputTag>("eventSelection")),
          evtSelToken_(consumes<bool>(evtSelSrc_)),
          doEvtSel_(conf.getParameter<bool>("doEventSelection")) {
    produces<std::vector<jet>>();
}

template <typename T>
void FullEventJetProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<double>("minPartPt");
  desc.add<double>("maxEta");
  desc.add<unsigned>("maxNumPart");

  desc.add<edm::InputTag>("eventSelection");
  desc.add<bool>("doEventSelection");

  desc.add<edm::InputTag>("partSrc");

  desc.add<int>("verbose");

  descriptions.addWithDefaultLabel(desc);
}

template <typename T>
void FullEventJetProducerT<T>::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of FullEventJetProducerT<T>::produce()\n");
    }
  edm::Handle<edm::View<T>> parts;
  evt.getByToken(partToken_, parts);

  auto result = std::make_unique<std::vector<jet>>();

  if(doEvtSel_){
    edm::Handle<bool> evtSel;
    evt.getByToken(evtSelToken_, evtSel);
    if(!*evtSel){
        evt.put(std::move(result));
        return;
    }
    if(verbose_){
        printf("passed event selection\n");
    }
  }

  jet ans;
  ans.pt = -999;
  ans.eta = 0.0;
  ans.phi = 0.0;
  ans.nPart = 0;
  ans.sumpt = 0.0;
  ans.iJet = 0;
  ans.particles.clear();

  for(size_t iPart=0; iPart < parts->size(); ++iPart){
      const auto& part = parts->at(iPart);

      if (ans.nPart < maxNumPart_){
          ans.sumpt += part.pt();
          if (part.pt() > minPartPt_ && std::abs(part.eta()) < maxEta_){
              particle next(part.pt(), part.eta(), part.phi(), 
                              0.0, 0.0, 0.0,
                              std::abs(part.pdgId()), part.charge());
              ans.particles.push_back(next);
              ans.nPart++;
          }
      }
  }

  std::sort(ans.particles.begin(), ans.particles.end(), [](const particle& a, const particle& b){
      return a.pt > b.pt;
  });

  if(verbose_){
      printf("Made full event jet with %lu particles\n", ans.particles.size());
  }

  result->push_back(std::move(ans));

  if(verbose_){
      printf("pushed back\n");
  }

  evt.put(std::move(result));
  if(verbose_){
      printf("put into event\n");
  }
}  // end produce()

typedef FullEventJetProducerT<reco::Candidate> RecoFullEventJetProducer;
typedef FullEventJetProducerT<reco::GenParticle> GenFullEventJetProducer;
typedef FullEventJetProducerT<reco::Candidate> CandidateFullEventJetProducer;

DEFINE_FWK_MODULE(RecoFullEventJetProducer);
DEFINE_FWK_MODULE(GenFullEventJetProducer);
DEFINE_FWK_MODULE(CandidateFullEventJetProducer);
