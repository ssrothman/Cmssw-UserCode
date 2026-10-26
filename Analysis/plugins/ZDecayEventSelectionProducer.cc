#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/MuonReco/interface/Muon.h"

#include "SRothman/SimonTools/src/jet.h"
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
#include "DataFormats/Candidate/interface/CompositeCandidate.h"

#include <iostream>
#include <memory>
#include <vector>

template <typename T>
class ZMuMuEventSelectionFilterT : public edm::stream::EDFilter<> {
public:
    explicit ZMuMuEventSelectionFilterT(const edm::ParameterSet&);
    ~ZMuMuEventSelectionFilterT() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    bool filter(edm::Event&, const edm::EventSetup&) override;
private:
    bool findGoodPair(const std::vector<T>& matchedId, unsigned&, unsigned&);
    bool pairIsGood(const T& p1, const T& p2);

    double leadPt_;
    double subPt_;

    double minZmass_;
    double maxZmass_;

    std::vector<int> pdgIds_;

    bool oppositeSign_;

    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<T>> srcToken_;

    int verbose_;

    bool onlyCheckLeading_;

    bool saveParticles_;
};

template <typename T>
ZMuMuEventSelectionFilterT<T>::ZMuMuEventSelectionFilterT(const edm::ParameterSet& conf)
        : leadPt_(conf.getParameter<double>("leadPt")),
          subPt_(conf.getParameter<double>("subPt")),
          minZmass_(conf.getParameter<double>("minZmass")),
          maxZmass_(conf.getParameter<double>("maxZmass")),
          pdgIds_(conf.getParameter<std::vector<int>>("pdgIds")),
          oppositeSign_(conf.getParameter<bool>("oppositeSign")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<T>>(src_)),
          verbose_(conf.getParameter<int>("verbose")),
          onlyCheckLeading_(conf.getParameter<bool>("onlyCheckLeading")),
          saveParticles_(conf.getParameter<bool>("saveParticles")){
    produces<bool>();
    if(saveParticles_){
        produces<std::vector<T>>("daughters");
        produces<std::vector<reco::CompositeCandidate>>("Z");
    }
}

template <typename T>
void ZMuMuEventSelectionFilterT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<double>("leadPt");
  desc.add<double>("subPt");
  desc.add<double>("minZmass");
  desc.add<double>("maxZmass");
  desc.add<std::vector<int>>("pdgIds");
  desc.add<bool>("oppositeSign");
  desc.add<edm::InputTag>("src");
  desc.add<int>("verbose");
  desc.add<bool>("onlyCheckLeading");
  desc.add<bool>("saveParticles");
  descriptions.addWithDefaultLabel(desc);
}

template <typename T>
bool ZMuMuEventSelectionFilterT<T>::pairIsGood(const T& p1, const T& p2){
    if(oppositeSign_ && p1.charge() * p2.charge() > 0){
        return false;
    }
    if(p1.pt() < leadPt_ || p2.pt() < subPt_){
        return false;
    }

    if (verbose_){
        printf("passed pt and charge\n");
        printf("leading particle: pt %f, eta %f, phi %f, charge %d\n", p1.pt(), p1.eta(), p1.phi(), p1.charge());
        printf("subleading particle: pt %f, eta %f, phi %f, charge %d\n", p2.pt(), p2.eta(), p2.phi(), p2.charge());
        fflush(stdout);
    }
    const auto& Z = p1.p4() + p2.p4();
    if (verbose_){
        printf("Z mass: %f\n", Z.mass());
        fflush(stdout);
    }
    if(Z.mass() > minZmass_ && Z.mass() < maxZmass_){
        return true;
    }
    return false;
}

template <typename T>
bool ZMuMuEventSelectionFilterT<T>::findGoodPair(const std::vector<T>& matchedId, unsigned& ip1, unsigned& ip2){
    if (onlyCheckLeading_){
        ip1 = 0;
        ip2 = 1;
        return pairIsGood(matchedId[0], matchedId[1]);
    }
    for (ip1=0; ip1<matchedId.size()-1; ++ip1){
        for(ip2=ip1+1; ip2<matchedId.size(); ++ip2){
            const auto& p1 = matchedId[ip1];
            const auto& p2 = matchedId[ip2];
            if(pairIsGood(p1, p2)){
                return true;
            }
        }
    }
    return false;
}

template <typename T>
bool ZMuMuEventSelectionFilterT<T>::filter(edm::Event& evt, const edm::EventSetup& setup){
    if(verbose_){
        printf("Top of ZMuMuEventSelectionFilterT::filter()\n");
        fflush(stdout);
    }

    edm::Handle<edm::View<T>> genParticles;
    evt.getByToken(srcToken_, genParticles);

    auto result = std::make_unique<bool>(false);
    auto result_parts = std::make_unique<std::vector<T>>();
    auto result_Z = std::make_unique<std::vector<reco::CompositeCandidate>>();
    
    for (const int& testPdgId : pdgIds_){
        if(verbose_){
            printf("testPdgId: %d\n", testPdgId);
            fflush(stdout);
        }
        std::vector<T> matchedId;
        for (const auto& genpart : *genParticles){
            if(std::abs(genpart.pdgId()) != testPdgId){
                continue;
            }
            matchedId.push_back(genpart);
        }
        if(matchedId.size() < 2){
            continue;
        }
        std::sort(matchedId.begin(), matchedId.end(), [](const T& a, const T& b){
            return a.pt() > b.pt();
        });

        unsigned ip1, ip2;
        *result = findGoodPair(matchedId, ip1, ip2);
        if (*result){
            result_parts->push_back(matchedId[ip1]);
            result_parts->push_back(matchedId[ip2]);
            const auto& p1 = matchedId[ip1];
            const auto& p2 = matchedId[ip2];
            reco::CompositeCandidate Z;
            Z.addDaughter(p1);
            Z.addDaughter(p2);
            AddFourMomenta addP4;
            addP4.set(Z);
            result_Z->push_back(Z);
            break;
        }
    }

    bool passed = *result;
    if(verbose_){
        if(*result){
            printf("ACCEPT\n");
        } else {
            printf("REJECT\n");
        }
        fflush(stdout);
    }
    evt.put(std::move(result));
    if(saveParticles_){
        evt.put(std::move(result_parts), "daughters");
        evt.put(std::move(result_Z), "Z");
    }
    if(verbose_){
        printf("put into event\n");
        fflush(stdout);
    }
    return passed;
}

#include "DataFormats/Candidate/interface/LeafCandidate.h"
using LeafCandidateZMuMuFilter = ZMuMuEventSelectionFilterT<reco::LeafCandidate>;
DEFINE_FWK_MODULE(LeafCandidateZMuMuFilter);

#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
using PackedGenParticleZMuMuFilter = ZMuMuEventSelectionFilterT<pat::PackedGenParticle>;
DEFINE_FWK_MODULE(PackedGenParticleZMuMuFilter);
