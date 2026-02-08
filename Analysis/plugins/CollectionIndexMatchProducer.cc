#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/Candidate/interface/LeafCandidate.h"
#include "DataFormats/Common/interface/ValueMap.h"

#include <memory>
#include <vector>
#include <cmath>

template<typename T>
class CollectionIndexMatchProducerT : public edm::stream::EDProducer<> {
public:
    explicit CollectionIndexMatchProducerT(const edm::ParameterSet&);
    ~CollectionIndexMatchProducerT() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    bool samePart(const T& p1, const T& p2) const;

    edm::InputTag src_;
    edm::InputTag searchCollection_;
    
    edm::EDGetTokenT<edm::View<T>> srcToken_;
    edm::EDGetTokenT<edm::View<T>> searchToken_;

    double ptTol_;
    double etaTol_;
    double phiTol_;

    bool requirePdgIdMatch_;
    bool requireChargeMatch_;
};

template<typename T>
CollectionIndexMatchProducerT<T>::CollectionIndexMatchProducerT(const edm::ParameterSet& conf)
    : src_(conf.getParameter<edm::InputTag>("src")),
      searchCollection_(conf.getParameter<edm::InputTag>("searchCollection")),
      srcToken_(consumes<edm::View<T>>(src_)),
      searchToken_(consumes<edm::View<T>>(searchCollection_)),
      ptTol_(conf.getParameter<double>("ptTolerance")),
      etaTol_(conf.getParameter<double>("etaTolerance")),
      phiTol_(conf.getParameter<double>("phiTolerance")),
      requirePdgIdMatch_(conf.getParameter<bool>("requirePdgIdMatch")),
      requireChargeMatch_(conf.getParameter<bool>("requireChargeMatch")) {
    produces<edm::ValueMap<int>>();
}

template<typename T>
void CollectionIndexMatchProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src", edm::InputTag(""));
    desc.add<edm::InputTag>("searchCollection", edm::InputTag(""));
    desc.add<double>("ptTolerance", 1e-3);
    desc.add<double>("etaTolerance", 1e-3);
    desc.add<double>("phiTolerance", 1e-3);
    desc.add<bool>("requirePdgIdMatch", true);
    desc.add<bool>("requireChargeMatch", true);
    descriptions.addWithDefaultLabel(desc);
}

template<typename T>
bool CollectionIndexMatchProducerT<T>::samePart(const T& p1, const T& p2) const {
    if (requirePdgIdMatch_ && std::abs(p1.pdgId()) != std::abs(p2.pdgId())) {
        return false;
    }
    if (requireChargeMatch_ && p1.charge() != p2.charge()) {
        return false;
    }
    if (std::abs(p1.pt() - p2.pt()) < ptTol_ &&
        std::abs(p1.eta() - p2.eta()) < etaTol_ &&
        std::abs(p1.phi() - p2.phi()) < phiTol_) {
        return true;
    }
    return false;
}

template<typename T>
void CollectionIndexMatchProducerT<T>::produce(edm::Event& evt, const edm::EventSetup& setup) {
    edm::Handle<edm::View<T>> srcParticles;
    edm::Handle<edm::View<T>> searchParticles;

    evt.getByToken(srcToken_, srcParticles);
    evt.getByToken(searchToken_, searchParticles);

    auto result = std::make_unique<edm::ValueMap<int>>();
    edm::ValueMap<int>::Filler filler(*result);

    std::vector<int> indices;
    for (const auto& srcPart : *srcParticles) {
        bool foundMatch = false;
        for (unsigned i = 0; i < searchParticles->size(); ++i) {
            const auto& searchPart = searchParticles->at(i);

            if (samePart(searchPart, srcPart)) {
                indices.push_back(i);
                foundMatch = true;
                break;
            }
        }
        if (!foundMatch) {
            indices.push_back(-1);
        }
    }

    filler.insert(srcParticles, indices.begin(), indices.end());
    filler.fill();
    evt.put(std::move(result));
}

using LeafCandidateCollectionIndexProducer = CollectionIndexMatchProducerT<reco::LeafCandidate>;

DEFINE_FWK_MODULE(LeafCandidateCollectionIndexProducer);
