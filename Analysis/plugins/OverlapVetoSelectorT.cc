#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

template <typename MainT, typename VetoerT>
class OverlapVetoSelectorT : public edm::stream::EDFilter<> {
public:
    explicit OverlapVetoSelectorT(const edm::ParameterSet&);
    ~OverlapVetoSelectorT() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    bool filter(edm::Event&, const edm::EventSetup&) override;

private:
    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<MainT>> srcToken_;

    edm::InputTag vetoer_;
    edm::EDGetTokenT<edm::View<VetoerT>> vetoerToken_;

    double minDeltaR_;

    bool filter_;

    int verbose_;
};

template <typename MainT, typename VetoerT>
OverlapVetoSelectorT<MainT, VetoerT>::OverlapVetoSelectorT(const edm::ParameterSet& conf)
        : src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<MainT>>(src_)),
          vetoer_(conf.getParameter<edm::InputTag>("vetoer")),
          vetoerToken_(consumes<edm::View<VetoerT>>(vetoer_)),
          minDeltaR_(conf.getParameter<double>("minDeltaR")),
          filter_(conf.getParameter<bool>("filter")),
          verbose_(conf.getParameter<int>("verbose"))
{
    produces<std::vector<MainT>>();
}

template <typename MainT, typename VetoerT>
void OverlapVetoSelectorT<MainT, VetoerT>::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src");
    desc.add<edm::InputTag>("vetoer");
    desc.add<double>("minDeltaR");
    desc.add<bool>("filter");
    desc.add<int>("verbose");
    descriptions.addWithDefaultLabel(desc);
}

template <typename MainT, typename VetoerT>
bool OverlapVetoSelectorT<MainT, VetoerT>::filter(edm::Event& iEvent, const edm::EventSetup& iSetup){
    edm::Handle<edm::View<MainT>> src;
    iEvent.getByToken(srcToken_, src);

    edm::Handle<edm::View<VetoerT>> vetoer;
    iEvent.getByToken(vetoerToken_, vetoer);

    auto result = std::make_unique<std::vector<MainT>>();
    int n = 0;

    for (const auto& s : *src){
        bool keep = true;
        for (const auto& v : *vetoer){
            if (reco::deltaR(s, v) < minDeltaR_){
                keep = false;
                break;
            }
        }
        if (keep){
            result->push_back(s);
            ++n;
        }
    }

    iEvent.put(std::move(result));

    if (filter_){
        return n > 0;
    } else {
        return true;
    }
}

using JetOverlapCandidateVetoSelector = OverlapVetoSelectorT<reco::Jet, reco::Candidate>;
using PATJetOverlapCandidateVetoSelector = OverlapVetoSelectorT<pat::Jet, reco::Candidate>;

DEFINE_FWK_MODULE(JetOverlapCandidateVetoSelector);
DEFINE_FWK_MODULE(PATJetOverlapCandidateVetoSelector);
