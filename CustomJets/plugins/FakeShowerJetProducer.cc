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

#include "SRothman/CustomJets/plugins/AddParticle.h"
#include "SRothman/SimonTools/src/isID.h"
#include "SRothman/SimonTools/src/particleThresholds.h"
#include "SRothman/SimonTools/src/partSyst.h"
#include "SRothman/SimonTools/src/ToyShowerer.h"

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
    bool passPtEtaPhi(const T& jet);
    bool passLepVeto(const T& jet);
    
    unsigned int maxNumPart_, minNumPart_;
    
    double minPt_;
    double maxEta_;

    double maxMuFrac_;
    double maxChEmFrac_;

    edm::InputTag jetSrc_;
    edm::EDGetTokenT<edm::View<T>> jetSrcToken_;

    int verbose_;

    std::string phi_mode_;
    std::string z_mode_;
    std::string theta_mode_;
    double zcut_;
    double theta_min_, theta_max_;
    ToyShowerer showerer_;
};

template <typename T>
FakeShowerJetProducerT<T>::FakeShowerJetProducerT(const edm::ParameterSet& conf)
        : maxNumPart_(conf.getParameter<unsigned>("maxNumPart")),
          minNumPart_(conf.getParameter<unsigned>("minNumPart")),
          minPt_(conf.getParameter<double>("minPt")),
          maxEta_(conf.getParameter<double>("maxEta")),
          maxMuFrac_(conf.getParameter<double>("maxMuFrac")),
          maxChEmFrac_(conf.getParameter<double>("maxChEmFrac")),
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
    produces<std::vector<jet>>();
}

template <typename T>
void FakeShowerJetProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<unsigned>("maxNumPart");
  desc.add<unsigned>("minNumPart");

  desc.add<double>("minPt");
  desc.add<double>("maxEta");
  desc.add<double>("maxMuFrac");
  desc.add<double>("maxChEmFrac");

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
bool FakeShowerJetProducerT<T>::passPtEtaPhi(const T& jet){
    return jet.pt() > minPt_ && std::fabs(jet.eta()) < maxEta_;
}

template <typename T>
bool FakeShowerJetProducerT<T>::passLepVeto(const T& jet){
    double chEmFrac=0;
    double muFrac=0;

    if constexpr(std::is_same<T, pat::Jet>::value){
        chEmFrac = jet.chargedEmEnergyFraction();
        muFrac = jet.muonEnergyFraction();
    } else if constexpr(std::is_same<T, reco::GenJet>::value){
        chEmFrac = jet.chargedEmEnergy() / jet.energy();
        muFrac = jet.muonEnergy() / jet.energy();
    } else {
        throw std::logic_error("FakeShowerJetProducer: unknown jet type");
    }

    return (chEmFrac < maxChEmFrac_) && (muFrac < maxMuFrac_);
}

template <typename T>
void FakeShowerJetProducerT<T>::produce(edm::Event& evt, 
                                   const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of FakeShowerJetProducerT<T>::produce()\n");
    }
    edm::Handle<edm::View<T>> jets;
    evt.getByToken(jetSrcToken_, jets);

    auto result = std::make_unique<std::vector<jet>>();

    if(verbose_){
        printf("passed event selection\n");
    }

    for(unsigned iJet=0; iJet < jets->size(); ++iJet){//for each jet
        const auto& j = jets->at(iJet);

        if(!passPtEtaPhi(j) || !passLepVeto(j)){
            continue;
        }
        
        const auto& constituents = j.getJetConstituents();

        if(constituents.size() < minNumPart_){
            continue;
        }

        if(verbose_){
            printf("jet %d passed jet selection\n", iJet);
        }

        jet ans;
        ans.iJet = iJet;

        showerer_.shower(j.pt(), j.eta(), j.phi(), j.mass(),
                constituents.size(), ans);

        if (verbose_){
            printf("\tjet: (%f, %f, %f)\n", ans.pt, ans.eta, ans.phi);
        }

        if(verbose_){
            printf("rawPt = %f\n", ans.rawpt);
            printf("pt = %f\n", ans.pt);
            printf("sumpt = %f\n", ans.sumpt);
            printf("jec = %f\n", ans.jecfactor);
        }

        if(ans.nPart >= minNumPart_){
            result->push_back(std::move(ans));
        }

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
