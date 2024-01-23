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

#include "RecoJets/JetProducers/interface/JetSpecific.h"

#include "SRothman/SimonTools/src/jets.h"
#include "SRothman/SimonTools/src/util.h"

#include <iostream>
#include <memory>
#include <vector>

static bool same(const reco::CandidatePtr& cand, 
        const pat::PackedCandidate& packedCand){
    return (std::abs(packedCand.eta() - cand->eta())<1e-5) &&
           (std::abs(packedCand.phi() - cand->phi())<1e-5) &&
           packedCand.pdgId() == cand->pdgId();
}

static int fromPV(const reco::CandidatePtr& cand, const edm::Handle<edm::View<pat::PackedCandidate>>& packedCands){
    for (const auto& packedCand : *packedCands){
        if (same(cand, packedCand)){
            return packedCand.fromPV();
        }
    }
    return 9999; 
}

static double puppiWeight(const reco::CandidatePtr& cand, const edm::Handle<edm::View<pat::PackedCandidate>>& packedCands){
    for (const auto& packedCand : *packedCands){
        if (same(cand, packedCand)){
            return packedCand.puppiWeight();
        }
    }
    return 9999; 
}

static double puppiPt(const reco::CandidatePtr& cand, const edm::Handle<edm::View<pat::PackedCandidate>>& packedCands){
    for (const auto& packedCand : *packedCands){
        if (same(cand, packedCand)){
            return packedCand.pt() * packedCand.puppiWeight();
        }
    }
    throw std::runtime_error("puppiPt: no matching packed candidate found");
    return 9999; 
}

static double puppiEnergy(const reco::CandidatePtr& cand, const edm::Handle<edm::View<pat::PackedCandidate>>& packedCands){
    for (const auto& packedCand : *packedCands){
        if (same(cand, packedCand)){
            return packedCand.energy() * packedCand.puppiWeight();
        }
    }
    throw std::runtime_error("puppiEnergy: no matching packed candidate found");
    return 9999; 
}

static math::PtEtaPhiMLorentzVector puppiP4(const reco::CandidatePtr& cand, const edm::Handle<edm::View<pat::PackedCandidate>>& packedCands){
    for (const auto& packedCand : *packedCands){
        if (same(cand, packedCand)){
            //printf("mass is %g\n", packedCand.mass());
            return math::PtEtaPhiMLorentzVector(
                    packedCand.pt() * packedCand.puppiWeight(),
                    packedCand.eta(), 
                    packedCand.phi(), 
                    packedCand.mass());
        }
    }
    throw std::runtime_error("puppiP4: no matching packed candidate found");
}


class PatJetTestProducer : public edm::stream::EDProducer<> {
public:
    explicit PatJetTestProducer(const edm::ParameterSet&);
    ~PatJetTestProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    int verbose_;

    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<pat::Jet>> srcToken_;

    edm::InputTag packedCandSrc_;
    edm::EDGetTokenT<edm::View<pat::PackedCandidate>> packedCandToken_;
};

PatJetTestProducer::PatJetTestProducer(const edm::ParameterSet& conf)
        : verbose_(conf.getParameter<int>("verbose")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<pat::Jet>>(src_)),
          packedCandSrc_(conf.getParameter<edm::InputTag>("packedCandSrc")),
          packedCandToken_(consumes<edm::View<pat::PackedCandidate>>(packedCandSrc_)){
    produces<std::vector<jet>>();
}

void PatJetTestProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<edm::InputTag>("src");
  desc.add<edm::InputTag>("packedCandSrc");
  desc.add<int>("verbose");

  descriptions.addWithDefaultLabel(desc);
}

void PatJetTestProducer::produce(edm::Event& evt, const edm::EventSetup& setup) { 
  printf("\n-----------------------------------------\n");
  printf("top of produce\n");
  fflush(stdout);

  edm::Handle<edm::View<pat::Jet>> jets;
  evt.getByToken(srcToken_, jets);

  edm::Handle<edm::View<pat::PackedCandidate>> packedCands;
  evt.getByToken(packedCandToken_, packedCands);

  auto result = std::make_unique<std::vector<jet>>();

  for(unsigned iJet=0; iJet < jets->size(); ++iJet){
    printf("\nJet %u\n", iJet);
    printf("available JEC levels: ");
    for(const auto& level : jets->at(iJet).availableJECLevels()){
        printf("%s ", level.c_str());
    }
    printf("\n");
    printf("available userfloats: ");
    for(const auto& name : jets->at(iJet).userFloatNames()){
        printf("%s ", name.c_str());
    }
    printf("\n");
    printf("available userints: ");
    for(const auto& name : jets->at(iJet).userIntNames()){
        printf("%s ", name.c_str());
    }
    printf("\n");
    printf("all available userdata: ");
    for(const auto& name : jets->at(iJet).userDataNames()){
        printf("%s ", name.c_str());
    }
    printf("\n");
    printf("discriminators: ");
    for(const auto& discpari : jets->at(iJet).getPairDiscri()){
        printf("%s ", discpari.first.c_str());
    }
    printf("\n");

    const auto& j = jets->at(iJet);
    if(std::abs(j.eta()) > 2.0) continue;
    printf("\tpt: %0.5f\n", j.pt());
    printf("\t\traw (M, E, pT, eta, phi) = (%0.5f, %0.5f, %0.5f, %0.5f, %0.5f)\n", j.mass(), 
                                                                                   j.energy()*j.jecFactor("Uncorrected"), 
                                                                                   j.pt()*j.jecFactor("Uncorrected"),
                                                                                   j.eta(), j.phi());
    printf("\tchEmEF: %0.5f\n", j.chargedEmEnergyFraction());
    printf("\tchHEF: %0.5f\n", j.chargedHadronEnergyFraction());
    printf("\tmuEF: %0.5f\n", j.muonEnergyFraction());
    printf("\tsum EF: %0.5f\n", j.chargedEmEnergyFraction() + 
                                j.chargedHadronEnergyFraction() +
                                j.neutralEmEnergyFraction() +
                                j.neutralHadronEnergyFraction() +
                                j.muonEnergyFraction());

    printf("\twith jetconstituents...\n");
    const std::vector<reco::CandidatePtr>& constituents1 = j.getJetConstituents();

    double sumpt=0, sumE=0, chEM=0, chH=0, mu=0;
    math::PtEtaPhiMLorentzVector P4(0, 0, 0, 0);
    for(const auto& candptr : constituents1){
        const auto* packedptr = dynamic_cast<const pat::PackedCandidate*>(candptr.get());
        if(packedptr){
            printf("GOT PACKEDPTR\n");
            printf("puppiweight is %g\n", packedptr->puppiWeight());
        } else{
            printf("Dynamic Cast failed\n");  
        }
        sumpt += candptr->pt();
        sumE += candptr->energy();
        P4 += candptr->polarP4();
        if(std::abs(candptr->pdgId()) == 11){
            chEM += candptr->energy();
        } else if(std::abs(candptr->pdgId()) == 13){
            mu += candptr->energy();
        } else if(std::abs(candptr->pdgId()) >= 100 && candptr->charge()!=0){
            chH += candptr->energy();
        }
    }

    printf("\t\trawpt = %0.5f\n", sumpt);
    printf("\t\ttotalE = %0.5f\n", sumE);
    printf("\t\tP4 (M, E, pT, eta, phi) = (%0.5f, %0.5f, %0.5f, %0.5f, %0.5f)\n", P4.M(), P4.energy(), P4.pt(), P4.eta(), P4.phi());
    printf("\t\tchEMEF = %0.5f\n", chEM/sumE);
    printf("\t\tchHEF = %0.5f\n", chH/sumE);
    printf("\t\tmuEF = %0.5f\n", mu/sumE);
    printf("\t\trawpt/jetraw = %0.5f\n", sumpt/(j.pt()*j.jecFactor("Uncorrected")));

    /*
    printf("\twith getPFConstituents...\n");
    
    sumpt=0; chEM=0; chH=0; mu=0;
    const std::vector<reco::PFCandidatePtr>& constituents2 = j.getPFConstituents();
    for(const auto& candptr : constituents2){
        sumpt += candptr->pt();
        if(std::abs(candptr->pdgId()) == 11){
            chEM += candptr->pt();
        } else if(std::abs(candptr->pdgId()) == 13){
            mu += candptr->pt();
        } else if(std::abs(candptr->pdgId()) >= 100 && candptr->charge()!=0){
            chH += candptr->pt();
        }
    }
    printf("\t\trawpt = %0.5f\n", sumpt);
    printf("\t\tchEMEF = %0.5f\n", chEM/sumpt);
    printf("\t\tchHEF = %0.5f\n", chH/sumpt);
    printf("\t\tmuEF = %0.5f\n", mu/sumpt);*/

    printf("\twith puppi from packedPFConstituents\n");
    sumpt=0; sumE=0; chEM=0; chH=0; mu=0;
    P4 = math::PtEtaPhiMLorentzVector(0, 0, 0, 0);
    const std::vector<reco::CandidatePtr>& constituents3 = j.daughterPtrVector();
    for(const auto& candptr : constituents3){
        double pupPt = puppiPt(candptr, packedCands);
        double pupE = puppiEnergy(candptr, packedCands);
        sumpt += pupPt;
        sumE += pupE;
        const auto pupP4 = puppiP4(candptr, packedCands);
        //std::cout << P4 << std::endl;
        //printf("plus ");
        //std::cout << pupP4 << std::endl;
        //printf("equals ");
        P4 += pupP4;
        //std::cout << P4 << std::endl;
        //printf("\n");
        if(std::abs(candptr->pdgId()) == 11){
            chEM += pupE;
        } else if(std::abs(candptr->pdgId()) == 13){
            mu += pupE;
        } else if(std::abs(candptr->pdgId()) >= 100 && candptr->charge()!=0){
            chH += pupE;
        } 
    }
    printf("\t\trawpt = %0.5f\n", sumpt);
    printf("\t\tP4 (M, E, pT, eta, phi) = (%0.5f, %0.5f, %0.5f, %0.5f, %0.5f)\n", P4.M(), P4.energy(), P4.pt(), P4.eta(), P4.phi());
    printf("\t\tchEMEF = %0.5f\n", chEM/sumE);
    printf("\t\tchHEF = %0.5f\n", chH/sumE);
    printf("\t\tmuEF = %0.5f\n", mu/sumE);
    printf("\t\trawpt/jetraw = %0.5f\n", sumpt/(j.pt()*j.jecFactor("Uncorrected")));
    
    printf("\twith PFspecific...\n");
    fflush(stdout);

    reco::PFJet::Specific spec;
    if(reco::makeSpecific(j.getJetConstituents(), &spec)){
        double totalE = spec.mChargedHadronEnergy + 
                        spec.mNeutralHadronEnergy +
                        spec.mChargedEmEnergy + 
                        spec.mNeutralEmEnergy + 
                        spec.mMuonEnergy;
        printf("\t\ttotal energy 1: %0.5f\n", totalE);
        printf("\t\tchEMEF = %0.5f\n", spec.mChargedEmEnergy/totalE);
        printf("\t\tchHEF = %0.5f\n", spec.mChargedHadronEnergy/totalE);
        printf("\t\tmuEF = %0.5f\n", spec.mMuonEnergy/totalE);

    } else {
        printf("\t\tDidn't work :(\n");
    }
    fflush(stdout);

    jet ans;
    ans.pt = 0;
    ans.eta = 0;
    ans.phi = 0;
    ans.nPart = 0;
    ans.nEM0 = 0;
    ans.nHAD0 = 0;
    ans.nHADCH = 0;
    ans.nMU = 0;
    ans.nELE = 0;
    ans.iJet = iJet;
    ans.sumpt = 0;
    ans.particles.clear();
    result->push_back(std::move(ans));
  }  // end for jet
  evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(PatJetTestProducer);
