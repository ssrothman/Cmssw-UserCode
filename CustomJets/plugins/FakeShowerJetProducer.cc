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
#include "SRothman/SimonTools/src/ToyShowerer.h"
#include "SRothman/SimonTools/src/computeJetMass.h"

#include <iostream>
#include <memory>
#include <vector>
#include <random>

template <typename T>
class FakeShowerJetProducerT : public edm::stream::EDProducer<> {
public:
    explicit FakeShowerJetProducerT(const edm::ParameterSet&);
    ~FakeShowerJetProducerT() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    simon::particleSelector selector_;

    edm::InputTag jetSrc_;
    edm::EDGetTokenT<edm::View<T>> jetSrcToken_;

    int verbose_;

    std::string phi_mode_;
    std::string z_mode_;
    std::string theta_mode_;
    double zcut_;
    double theta_min_, theta_max_;
    simon::ToyShowerer showerer_;
};

template <typename T>
FakeShowerJetProducerT<T>::FakeShowerJetProducerT(const edm::ParameterSet& conf)
        : 
          selector_(conf.getParameter<edm::ParameterSet>("selector")),
          jetSrc_(conf.getParameter<edm::InputTag>("jetSrc")),
          jetSrcToken_(consumes<edm::View<T>>(jetSrc_)),
          verbose_(conf.getParameter<int>("verbose")),
          phi_mode_(conf.getParameter<std::string>("phi_mode")),
          z_mode_(conf.getParameter<std::string>("z_mode")),
          theta_mode_(conf.getParameter<std::string>("theta_mode")),
          zcut_(conf.getParameter<double>("zcut")),
          theta_min_(conf.getParameter<double>("theta_min")),
          theta_max_(conf.getParameter<double>("theta_max")),
          showerer_(phi_mode_, z_mode_, theta_mode_, 
                    zcut_, theta_min_, theta_max_){

    //std::string moduleName = conf.getParameter<std::string>("@module_label");
    //showerer_.enable_logging(moduleName + ".log");
    produces<std::vector<simon::jet>>();
}

template <typename T>
void FakeShowerJetProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  edm::ParameterSetDescription selectorDesc;
  simon::particleSelector::fillPSetDescription(selectorDesc);

  desc.add<int>("verbose");

  desc.add<edm::InputTag>("jetSrc");

  desc.add<std::string>("phi_mode");
  desc.add<std::string>("z_mode");
  desc.add<std::string>("theta_mode");
  desc.add<double>("zcut");
  desc.add<double>("theta_min");
  desc.add<double>("theta_max");

  descriptions.addWithDefaultLabel(desc);
}

template <typename T>
void FakeShowerJetProducerT<T>::produce(edm::Event& evt, 
                                   const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of FakeShowerJetProducerT<T>::produce()\n");
    }
    edm::Handle<edm::View<T>> jets;
    evt.getByToken(jetSrcToken_, jets);

    auto result = std::make_unique<std::vector<simon::jet>>();

    if(verbose_){
        printf("passed event selection\n");
    }

    for(unsigned iJet=0; iJet < jets->size(); ++iJet){//for each jet
        const auto& j = jets->at(iJet);

        const auto& constituents = j.getJetConstituents();

        simon::jet ans;
        ans.iJet = iJet;

        showerer_.shower(j.pt(), j.eta(), j.phi(),
                constituents.size(), ans);

        simon::computeJetMass(ans);

        if (verbose_){
            printf("\tjet: (%f, %f, %f)\n", ans.pt, ans.eta, ans.phi);
        }

        if(verbose_){
            printf("rawPt = %f\n", ans.rawpt);
            printf("pt = %f\n", ans.pt);
            printf("sumpt = %f\n", ans.sumpt);
            printf("jec = %f\n", ans.jecfactor);
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

typedef FakeShowerJetProducerT<pat::Jet> PatFakeShowerJetProducer;
typedef FakeShowerJetProducerT<reco::GenJet> GenFakeShowerJetProducer;

DEFINE_FWK_MODULE(PatFakeShowerJetProducer);
DEFINE_FWK_MODULE(GenFakeShowerJetProducer);
