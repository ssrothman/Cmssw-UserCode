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
    double minPartPt_;

    int verbose_;

    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<T>> srcToken_;
};

template <typename T>
SimonJetProducerT<T>::SimonJetProducerT(const edm::ParameterSet& conf)
        : minPartPt_(conf.getParameter<double>("minPartPt")),
          verbose_(conf.getParameter<int>("verbose")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<T>>(src_)){
    //produces<std::vector<jet>>();
    produces<teststruct>();
}

template <typename T>
void SimonJetProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<double>("minPartPt");
  desc.add<edm::InputTag>("src");
  desc.add<int>("verbose");
  descriptions.addWithDefaultLabel(desc);
}

template <typename T>
void SimonJetProducerT<T>::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<T>> jets;
  evt.getByToken(srcToken_, jets);

  auto result = std::make_unique<std::vector<jet>>();

  unsigned iJet=0;
  for (const T& j : *jets){

    const std::vector<reco::Jet::Constituent>& constituents = j.getJetConstituents();

    double pt = j.pt();
    double eta = j.eta();
    double phi = j.phi();

    jet ans;
    ans.pt = pt;
    ans.eta = eta;
    ans.phi = phi;
    ans.nPart = 0;
    ans.iJet = iJet++;

    if (verbose_){
      std::cout << "\tjet: (" << pt << ", " << eta << ", " << phi << ")" << std::endl;
    }

    for(const auto& part : j){
        double partpt = part.pt();
        ans.sumpt += partpt;
        if(partpt >= minPartPt_){
            ans.particles.emplace_back(part.pt(), part.eta(), part.phi(), 
                                       0.0, 0.0, 0.0,
                                       std::abs(part.pdgId()), part.charge());
            ++ans.nPart;
        }
    }
    result->emplace_back(ans);
  }  // end for jet
  evt.put(std::move(result));
}  // end produce()

typedef SimonJetProducerT<pat::Jet> PatSimonJetProducer;
typedef SimonJetProducerT<reco::PFJet> RecoSimonJetProducer;
typedef SimonJetProducerT<reco::GenJet> GenSimonJetProducer;

DEFINE_FWK_MODULE(PatSimonJetProducer);
DEFINE_FWK_MODULE(RecoSimonJetProducer);
DEFINE_FWK_MODULE(GenSimonJetProducer);
