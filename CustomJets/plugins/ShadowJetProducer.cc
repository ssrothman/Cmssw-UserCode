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

#include "SRothman/SimonTools/src/jets.h"
#include "SRothman/SimonTools/src/util.h"
#include "SRothman/CustomJets/plugins/AddParticle.h"
#include "SRothman/SimonTools/src/selectionStructs.h"

#include <iostream>
#include <memory>
#include <vector>

template <typename T>
class ShadowJetProducerT : public edm::stream::EDProducer<> {
public:
    explicit ShadowJetProducerT(const edm::ParameterSet&);
    ~ShadowJetProducerT() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    bool anyClose(const T& part, const std::vector<particle>& parts) const;

    struct particleThresholds thresholds_;
    struct vtxCuts vtxCuts_;

    bool onlyCharged_;

    unsigned int maxNumPart_, minNumPart_;
    
    bool applyPuppi_;

    double dR2window_;

    edm::InputTag partSrc_;
    edm::EDGetTokenT<edm::View<T>> partToken_;

    edm::InputTag jetSrc_;
    edm::EDGetTokenT<edm::View<jet>> jetToken_;

    edm::InputTag evtSelSrc_;
    edm::EDGetTokenT<bool> evtSelToken_;
    bool doEvtSel_;

    int verbose_;
};

template <typename T>
bool ShadowJetProducerT<T>::anyClose(const T& part, const std::vector<particle>& parts) const {
    for(const auto& p : parts){
        if(reco::deltaR2(part.eta(), part.phi(), p.eta, p.phi) < dR2window_){
            return true;
        }
    }
    return false;
}

template <typename T>
ShadowJetProducerT<T>::ShadowJetProducerT(const edm::ParameterSet& conf)
        : thresholds_(conf.getParameter<edm::ParameterSet>("thresholds")),
          vtxCuts_(conf.getParameter<edm::ParameterSet>("vtxCuts")),
          onlyCharged_(conf.getParameter<bool>("onlyCharged")),
          maxNumPart_(conf.getParameter<unsigned>("maxNumPart")),
          minNumPart_(conf.getParameter<unsigned>("minNumPart")),
          applyPuppi_(conf.getParameter<bool>("applyPuppi")),
          dR2window_(square(conf.getParameter<double>("dRwindow"))),
          partSrc_(conf.getParameter<edm::InputTag>("partSrc")),
          partToken_(consumes<edm::View<T>>(partSrc_)),
          jetSrc_(conf.getParameter<edm::InputTag>("jetSrc")),
          jetToken_(consumes<edm::View<jet>>(jetSrc_)),
          evtSelSrc_(conf.getParameter<edm::InputTag>("eventSelection")),
          evtSelToken_(consumes<bool>(evtSelSrc_)),
          doEvtSel_(conf.getParameter<bool>("doEventSelection")),
          verbose_(conf.getParameter<int>("verbose")){
    produces<std::vector<jet>>();
}

template <typename T>
void ShadowJetProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  edm::ParameterSetDescription thresholdDesc;
  particleThresholds::fillPSetDescription(thresholdDesc);
  desc.add<edm::ParameterSetDescription>(
            "thresholds", thresholdDesc);

  desc.add<bool>("onlyCharged");

  edm::ParameterSetDescription vtxDesc;
  vtxCuts::fillPSetDescription(vtxDesc);
  desc.add<edm::ParameterSetDescription>(
            "vtxCuts", vtxDesc);

  desc.add<unsigned>("maxNumPart");
  desc.add<unsigned>("minNumPart");

  desc.add<bool>("applyPuppi");

  desc.add<double>("dRwindow");

  desc.add<edm::InputTag>("eventSelection");
  desc.add<bool>("doEventSelection");

  desc.add<edm::InputTag>("partSrc");
  desc.add<edm::InputTag>("jetSrc");

  desc.add<int>("verbose");

  descriptions.addWithDefaultLabel(desc);
}

template <typename T>
void ShadowJetProducerT<T>::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of ShadowJetProducerT<T>::produce()\n");
    }
  edm::Handle<edm::View<T>> parts;
  evt.getByToken(partToken_, parts);

  edm::Handle<edm::View<jet>> jets;
  evt.getByToken(jetToken_, jets);

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

  for(unsigned iJet=0; iJet < jets->size(); ++iJet){

    const auto& j = jets->at(iJet);

    double pt = j.pt;
    double eta = j.eta;
    double phi = j.phi;

    jet ans;
    ans.pt = pt;
    ans.eta = eta;
    ans.phi = phi;
    ans.nPart = 0;
    ans.sumpt = 0;
    ans.iJet = iJet;
    ans.particles.clear();

    if (verbose_){
      std::cout << "\tjet: (" << pt << ", " << eta << ", " << phi << ")" << std::endl;
    }

    for(size_t iPart=0; iPart < parts->size(); ++iPart){
        const auto& part = parts->at(iPart);
        if(anyClose(part, j.particles)){
            const auto* partptr = dynamic_cast<const pat::PackedCandidate*>(&part);
            const auto* genptr = dynamic_cast<const pat::PackedGenParticle*>(&part);
            
            if(partptr){
                addParticle(partptr, ans, 1.0, 
                            applyPuppi_, false, 
                            onlyCharged_,
                            9999, thresholds_,
                            vtxCuts_,
                            maxNumPart_);
           } else if(genptr){
                addParticle(genptr, ans, 1.0, 
                            applyPuppi_, false,
                            onlyCharged_,
                            9999, thresholds_,
                            vtxCuts_,
                            maxNumPart_);
           } else {
                throw std::runtime_error("constituent is not a PackedCandidate or PackedGenCandidate");
            }
        } // end for part
    }

    std::sort(ans.particles.begin(), ans.particles.end(), [](const particle& a, const particle& b){
        return a.pt > b.pt;
    });

    if(ans.nPart < minNumPart_){
        continue;
    }
    result->push_back(std::move(ans));
    if(verbose_){
        printf("pushed back\n");
    }
  }  // end for jet
  evt.put(std::move(result));
  if(verbose_){
      printf("put into event\n");
  }
}  // end produce()

typedef ShadowJetProducerT<reco::Candidate> RecoShadowJetProducer;
typedef ShadowJetProducerT<reco::GenParticle> GenShadowJetProducer;
typedef ShadowJetProducerT<reco::Candidate> CandidateShadowJetProducer;

DEFINE_FWK_MODULE(RecoShadowJetProducer);
DEFINE_FWK_MODULE(GenShadowJetProducer);
DEFINE_FWK_MODULE(CandidateShadowJetProducer);
