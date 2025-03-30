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

#include "SRothman/SimonTools/src/jet.h"
#include "SRothman/SimonTools/src/util.h"
#include "SRothman/SimonTools/src/computeJetMass.h"
#include "SRothman/SimonTools/src/particleSelector.h"

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
    template <typename T2>
    bool anyClose(const T2& part, const std::vector<simon::particle>& parts) const{
        for(const auto& p : parts){
            if(reco::deltaR2(part.eta(), part.phi(), p.eta, p.phi) < dR2window_){
                return true;
            }
        }
        return false;
    }

    simon::particleSelector selector_;

    double dR2window_;

    edm::InputTag partSrc_;
    edm::EDGetTokenT<edm::View<T>> partToken_;

    edm::InputTag jetSrc_;
    edm::EDGetTokenT<edm::View<simon::jet>> jetToken_;

    int verbose_;
};

template <typename T>
ShadowJetProducerT<T>::ShadowJetProducerT(const edm::ParameterSet& conf)
        : selector_(conf.getParameter<edm::ParameterSet>("selector")),
          dR2window_(simon::square(conf.getParameter<double>("dRwindow"))),
          partSrc_(conf.getParameter<edm::InputTag>("partSrc")),
          partToken_(consumes<edm::View<T>>(partSrc_)),
          jetSrc_(conf.getParameter<edm::InputTag>("jetSrc")),
          jetToken_(consumes<edm::View<simon::jet>>(jetSrc_)),
          verbose_(conf.getParameter<int>("verbose")){
    produces<std::vector<simon::jet>>();
}

template <typename T>
void ShadowJetProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  edm::ParameterSetDescription selectorDesc;
  simon::particleSelector::fillPSetDescription(selectorDesc);
  desc.add<edm::ParameterSetDescription>("selector", selectorDesc);

  desc.add<double>("dRwindow");

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

  edm::Handle<edm::View<simon::jet>> jets;
  evt.getByToken(jetToken_, jets);

  auto result = std::make_unique<std::vector<simon::jet>>();

  if(verbose_){
      printf("passed event selection\n");
  }

  for(unsigned iJet=0; iJet < jets->size(); ++iJet){

    const auto& j = jets->at(iJet);

    double pt = j.pt;
    double eta = j.eta;
    double phi = j.phi;

    simon::jet ans;
    ans.pt = pt;
    ans.eta = eta;
    ans.phi = phi;
    ans.nPart = 0;
    ans.sumpt = 0;
    ans.iJet = iJet;
    ans.particles.clear();

    //printf("making ShadowJet jet with eta: %f, phi: %f\n", ans.eta, ans.phi);

    if (verbose_){
      std::cout << "\tjet: (" << pt << ", " << eta << ", " << phi << ")" << std::endl;
    }


    auto thefilter = [this, j](const edm::Ptr<reco::Candidate>& part){
        return anyClose(*part, j.particles);
    };
    

    selector_.buildJet(parts->ptrs(), ans, &thefilter);

    ans.pt = ans.rawpt;
    computeJetMass(ans);
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
