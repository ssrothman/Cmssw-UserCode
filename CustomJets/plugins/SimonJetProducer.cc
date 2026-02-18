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

    edm::Handle<edm::View<T>> CHSjets;
    if(addCHSindex_){
        evt.getByToken(CHSsrcToken_, CHSjets);
    }

    auto result = std::make_unique<std::vector<simon::jet>>();

    for(unsigned iJet=0; iJet < jets->size(); ++iJet){//for each jet
        const auto& j = jets->at(iJet);

        const auto& constituents = j.getJetConstituents();

        simon::jet ans;
        ans.pt = j.pt();
        ans.eta = j.eta();
        ans.phi = j.phi();
        ans.mass = j.mass();
        ans.iJet = iJet;

        if(addCHSindex_){
            for(unsigned iCHS=0; iCHS < CHSjets->size(); ++iCHS){
                const auto& jCHS = CHSjets->at(iCHS);
                if(deltaR(j, jCHS) < CHSmatchDR_){//if the CHS jet matches the jet
                    ans.iCHS.push_back(iCHS);
                }
            }
            if(ans.iCHS.empty()){
                ans.iCHS.push_back(99999999);
            }
        }

        selector_.buildJet(constituents, ans);

        if (verbose_){
            printf("\tjet: (%f, %f, %f)\n", ans.pt, ans.eta, ans.phi);
        }

        if(verbose_){
            printf("rawPt = %f\n", ans.rawpt);
            printf("pt = %f\n", ans.pt);
            printf("sumpt = %f\n", ans.sumpt);
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

typedef SimonJetProducerT<pat::Jet> PatSimonJetProducer;
typedef SimonJetProducerT<reco::GenJet> GenSimonJetProducer;

DEFINE_FWK_MODULE(PatSimonJetProducer);
DEFINE_FWK_MODULE(GenSimonJetProducer);
