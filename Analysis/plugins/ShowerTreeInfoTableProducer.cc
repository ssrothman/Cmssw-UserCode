#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "SRothman/SimonTools/src/lund.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>

namespace {
    // Structure to hold information about shower tree nodes
    struct ShowerTreeInfo {
        // Core particle properties
        float pt;
        float eta;
        float phi;
        float mass;
        int pdgId;
        int charge;
        
        // Tree structure properties
        int nDaughters;
        int nMothers;
        
        std::vector<int> mothers;
        std::vector<int> daughters;

        // Status and flags
        int status;
        
        ShowerTreeInfo() : 
            pt(0), eta(0), phi(0), mass(0), pdgId(0), charge(0),
            nDaughters(0), nMothers(0), 
            mothers(), daughters(),
            status(0) {}
    };
}

bool isGluon(const ShowerTreeInfo& j){
    return j.pdgId == 21;
}
bool isQuark(const ShowerTreeInfo& j){
    return std::abs(j.pdgId) <= 6 && std::abs(j.pdgId) >= 1;
}
bool isGluToGluGlu(const ShowerTreeInfo& j1, 
                   const ShowerTreeInfo& j2,
                   const ShowerTreeInfo& j3){

    return isGluon(j1) && isGluon(j2) && isGluon(j3);
}
bool isGluToQQ(const ShowerTreeInfo& j1, 
               const ShowerTreeInfo& j2,
               const ShowerTreeInfo& j3){

    return isGluon(j1) && isQuark(j2) && isQuark(j3);
}
bool isQToQGlu(const ShowerTreeInfo& j1, 
               const ShowerTreeInfo& j2,
               const ShowerTreeInfo& j3){

    return isQuark(j1) && isQuark(j2) && isGluon(j3);
}
bool isQToGluQ(const ShowerTreeInfo& j1, 
               const ShowerTreeInfo& j2,
               const ShowerTreeInfo& j3){

    return isQuark(j1) && isGluon(j2) && isQuark(j3);
}

int classifyOneSplitting(const ShowerTreeInfo& j1, 
                      const ShowerTreeInfo& j2,
                      const ShowerTreeInfo& j3){
    
    if(isGluToGluGlu(j1, j2, j3)){
        return 0;
    } else if (isGluToQQ(j1, j2, j3)){
        return 1;
    } else if (isQToQGlu(j1, j2, j3)){
        return 2;
    } else if (isQToGluQ(j1, j2, j3)){
        return 3;
    } else {
        return -1;
    }
}

struct somenumbers{
    double delta_R, z, kt, phi;
};
void get_single_info(const ShowerTreeInfo& j2,
                     const ShowerTreeInfo& j3,
                     struct somenumbers& result){
    
    result.delta_R = reco::deltaR(
        j2.eta, j2.phi,
        j3.eta, j3.phi
    );
    result.z = j3.pt / (j2.pt + j3.pt);
    result.kt = j3.pt * result.delta_R;

    result.phi = atan2(
        j2.eta - j3.eta,
        reco::deltaPhi(j2.phi, j3.phi)
    );
}

int classifyTwoSplittings(const ShowerTreeInfo& j1, 
                          const ShowerTreeInfo& j2,
                          const ShowerTreeInfo& j3,
                          const ShowerTreeInfo& j4,
                          const ShowerTreeInfo& j5){

    int split1 = classifyOneSplitting(j1, j2, j3);
    int split2 = classifyOneSplitting(j2, j4, j5);
    
    if (split1 < 0 || split2 < 0){
        return -1;
    } else {
        return split1 + 4 * split2;
    }
}

class ShowerTreeInfoTableProducer : public edm::stream::EDProducer<> {
public:
    explicit ShowerTreeInfoTableProducer(const edm::ParameterSet&);
    ~ShowerTreeInfoTableProducer() override {}

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    void produce(edm::Event&, const edm::EventSetup&) override;
    
    edm::InputTag genParticlesTag_;
    edm::EDGetTokenT<edm::View<reco::GenParticle>> genParticlesToken_;

    edm::InputTag genJetsTag_;
    edm::EDGetTokenT<edm::View<reco::GenJet>> genJetsToken_;
    
    int verbose_;
};

ShowerTreeInfoTableProducer::ShowerTreeInfoTableProducer(const edm::ParameterSet& iConfig) :
    genParticlesTag_(iConfig.getParameter<edm::InputTag>("src")),
    genParticlesToken_(consumes<edm::View<reco::GenParticle>>(genParticlesTag_)),
    genJetsTag_(iConfig.getParameter<edm::InputTag>("jets")),
    genJetsToken_(consumes<edm::View<reco::GenJet>>(genJetsTag_)),
    verbose_(iConfig.getParameter<int>("verbose"))
{
    produces<nanoaod::FlatTable>();
}

void ShowerTreeInfoTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src")->setComment("GenParticles collection");
    desc.add<edm::InputTag>("jets")->setComment("GenJets collection");
    desc.add<int>("verbose", 0)->setComment("Verbosity level");
    descriptions.addWithDefaultLabel(desc);
}

void ShowerTreeInfoTableProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {   
    edm::Handle<edm::View<reco::GenParticle>> genParticles;
    iEvent.getByToken(genParticlesToken_, genParticles);
    
    edm::Handle<edm::View<reco::GenJet>> genJets;
    iEvent.getByToken(genJetsToken_, genJets);

    // Build the shower tree information for each particle
    std::vector<ShowerTreeInfo> treeInfos;
    treeInfos.reserve(genParticles->size());
        
    for(size_t i = 0; i < genParticles->size(); ++i) {
        const auto& particle = genParticles->at(i);
        ShowerTreeInfo info;
        
        // Basic kinematic properties
        info.pt = particle.pt();
        info.eta = particle.eta();
        info.phi = particle.phi();
        info.mass = particle.mass();
        info.pdgId = particle.pdgId();
        info.charge = particle.charge();
        
        // Status and flags
        info.status = particle.status();
        
        // Tree structure properties
        info.nDaughters = particle.numberOfDaughters();
        info.nMothers = particle.numberOfMothers();
        
        // Populate mothers vector with indices
        info.mothers.reserve(info.nMothers);
        for(size_t m = 0; m < particle.numberOfMothers(); ++m) {
            const reco::Candidate* mother = particle.mother(m);
            if(mother) {
                // Find the index of this mother in the genParticles collection
                for(size_t j = 0; j < genParticles->size(); ++j) {
                    if(&(genParticles->at(j)) == mother) {
                        info.mothers.push_back(j);
                        break;
                    }
                }
            }
        }
        
        // Populate daughters vector with indices
        info.daughters.reserve(info.nDaughters);
        for(size_t d = 0; d < particle.numberOfDaughters(); ++d) {
            const reco::Candidate* daughter = particle.daughter(d);
            if(daughter) {
                // Find the index of this daughter in the genParticles collection
                for(size_t j = 0; j < genParticles->size(); ++j) {
                    if(&(genParticles->at(j)) == daughter) {
                        info.daughters.push_back(j);
                        break;
                    }
                }
            }
        }

        // Sort mothers and daughters by pT of referenced particles
        std::sort(info.mothers.begin(), info.mothers.end(), 
            [&genParticles](int a, int b) { 
            return genParticles->at(a).pt() > genParticles->at(b).pt(); 
            });
        
        std::sort(info.daughters.begin(), info.daughters.end(), 
            [&genParticles](int a, int b) { 
            return genParticles->at(a).pt() > genParticles->at(b).pt(); 
            });

        if(verbose_ > 1) {
            std::cout << "Particle " << i << ": PDG=" << info.pdgId 
                      << " pT=" << info.pt << " daughters=" << info.nDaughters 
                      << " mothers=" << info.nMothers;
        }
        
        treeInfos.push_back(info);
    }
    
    if(verbose_ > 0) {
        std::cout << "ShowerTreeInfoTableProducer: produced " << treeInfos.size() 
                  << " tree info entries" << std::endl;
    }

    std::vector<simon::SplittingInfo> splittings;
    std::vector<int> splittingTypes;

    // look up the hardest jet
    if (genJets->size() > 0){
        const auto& j0 = genJets->at(0);

        if (verbose_ > 0){
            printf("The hardest jet has (pt, eta, phi) = (%g, %f, %f)\n",
            j0.pt(), j0.eta(), j0.phi());
        }

        // now find the hardest initial-state particle 
        // nearby the hardest jet
        int hardest = -1;
        double hardest_pt = 0;
        for (int i=0; i<int(treeInfos.size()); ++i){
            const auto& ti = treeInfos[i];
            if (ti.status==1){ //reject final-state particles
                continue;
            }
            if (ti.status==2) { // reject hadrons, muons and taus
                continue;
            }
            if (ti.status==4) { //reject beam particles
                continue;
            }
            if (reco::deltaR(ti.eta, ti.phi, j0.eta(), j0.phi()) > 0.4){ // reject particles too far away from the jet
                continue;
            }

            if (ti.pt > hardest_pt){
                hardest = i;
                hardest_pt = ti.pt;
            }
        }

        if (verbose_ > 0){
            printf("Found an initial-state particle near the jet with (pt, eta, phi) = (%g, %f, %f)\n",
                treeInfos[hardest].pt, treeInfos[hardest].eta, treeInfos[hardest].phi);
            printf("\tand pdgid, charge = (%d, %d)\n",
                treeInfos[hardest].pdgId, treeInfos[hardest].charge);
        }

        //so now we traverse the splitting tree
        //in a "lund" way, each time taking the hardest branch
    
        ShowerTreeInfo j1, j2, j3, j4, j5;
        j1 = treeInfos[hardest];

        // loop until find a final-state particle
        // or a particle without any daughters
        while (j1.status != 1 && j1.nDaughters > 0){
            j2 = treeInfos[j1.daughters[0]];

            // skip over everything
            // that isn't a two-way splitting    
            if (j1.nDaughters != 2 || j2.nDaughters != 2){ 

                if (verbose_ > 1){
                    printf("skipping over a %d->%d splitting\n", 
                        j1.nDaughters,
                        j2.nDaughters);
                }

            } else {
                j2 = treeInfos[j1.daughters[0]];
                j3 = treeInfos[j1.daughters[1]];

                j4 = treeInfos[j2.daughters[0]];
                j5 = treeInfos[j2.daughters[1]];

                simon::SplittingInfo nextinfo;
                nextinfo.pt1 = j1.pt;
                nextinfo.pt2 = j2.pt;
                nextinfo.pt3 = j3.pt;
                nextinfo.pt4 = j4.pt;
                nextinfo.pt5 = j5.pt;
                
                struct somenumbers info23;
                get_single_info(j2, j3, info23);
                nextinfo.delta_R23 = info23.delta_R;
                nextinfo.z23 = info23.z;
                nextinfo.kt23 = info23.kt;
                nextinfo.phi23 = info23.phi;

                struct somenumbers info45;
                get_single_info(j4, j5, info45);
                nextinfo.delta_R45 = info45.delta_R;
                nextinfo.z45 = info45.z;
                nextinfo.kt45 = info45.kt;
                nextinfo.phi45 = info45.phi;

                nextinfo.deltaPsi = reco::deltaPhi(
                    nextinfo.phi23,
                    nextinfo.phi45
                );

                splittings.push_back(nextinfo);
                splittingTypes.push_back(
                    classifyTwoSplittings(
                        j1, j2, j3, j4, j5
                    )
                );
            }
            j1 = j2;
        }
    }
    
    // Create the NanoAOD FlatTable
    auto table = std::make_unique<nanoaod::FlatTable>(splittings.size(), "GenSplittings", false);
    {
        std::vector<float> pt1;
        std::vector<float> pt2;
        std::vector<float> pt3;
        std::vector<float> pt4;
        std::vector<float> pt5;

        std::vector<float> delta_R23;
        std::vector<float> z23;
        std::vector<float> kt23;
        std::vector<float> phi23;

        std::vector<float> delta_R45;
        std::vector<float> z45;
        std::vector<float> kt45;
        std::vector<float> phi45;

        std::vector<float> deltaPsi;
        std::vector<int> splitType;

        pt1.reserve(splittings.size());
        pt2.reserve(splittings.size());
        pt3.reserve(splittings.size());
        pt4.reserve(splittings.size());
        pt5.reserve(splittings.size());

        delta_R23.reserve(splittings.size());
        z23.reserve(splittings.size());
        kt23.reserve(splittings.size());
        phi23.reserve(splittings.size());

        delta_R45.reserve(splittings.size());
        z45.reserve(splittings.size());
        kt45.reserve(splittings.size());
        phi45.reserve(splittings.size());

        deltaPsi.reserve(splittings.size());
        splitType.reserve(splittings.size());

        for (size_t i = 0; i < splittings.size(); ++i){
            const auto& s = splittings[i];
            pt1.push_back(s.pt1);
            pt2.push_back(s.pt2);
            pt3.push_back(s.pt3);
            pt4.push_back(s.pt4);
            pt5.push_back(s.pt5);

            delta_R23.push_back(s.delta_R23);
            z23.push_back(s.z23);
            kt23.push_back(s.kt23);
            phi23.push_back(s.phi23);

            delta_R45.push_back(s.delta_R45);
            z45.push_back(s.z45);
            kt45.push_back(s.kt45);
            phi45.push_back(s.phi45);

            deltaPsi.push_back(s.deltaPsi);
            splitType.push_back(splittingTypes[i]);
        }

        table->addColumn<float>("pt1", pt1, "pT of node 1", nanoaod::FlatTable::FloatColumn);
        table->addColumn<float>("pt2", pt2, "pT of node 2", nanoaod::FlatTable::FloatColumn);
        table->addColumn<float>("pt3", pt3, "pT of node 3", nanoaod::FlatTable::FloatColumn);
        table->addColumn<float>("pt4", pt4, "pT of node 4", nanoaod::FlatTable::FloatColumn);
        table->addColumn<float>("pt5", pt5, "pT of node 5", nanoaod::FlatTable::FloatColumn);

        table->addColumn<float>("deltaR23", delta_R23, "DeltaR between nodes 2 and 3", nanoaod::FlatTable::FloatColumn);
        table->addColumn<float>("z23", z23, "z for nodes 2 and 3", nanoaod::FlatTable::FloatColumn);
        table->addColumn<float>("kt23", kt23, "kT for nodes 2 and 3", nanoaod::FlatTable::FloatColumn);
        table->addColumn<float>("phi23", phi23, "phi for nodes 2 and 3", nanoaod::FlatTable::FloatColumn);

        table->addColumn<float>("deltaR45", delta_R45, "DeltaR between nodes 4 and 5", nanoaod::FlatTable::FloatColumn);
        table->addColumn<float>("z45", z45, "z for nodes 4 and 5", nanoaod::FlatTable::FloatColumn);
        table->addColumn<float>("kt45", kt45, "kT for nodes 4 and 5", nanoaod::FlatTable::FloatColumn);
        table->addColumn<float>("phi45", phi45, "phi for nodes 4 and 5", nanoaod::FlatTable::FloatColumn);

        table->addColumn<float>("deltaPsi", deltaPsi, "deltaPsi between splittings", nanoaod::FlatTable::FloatColumn);
        table->addColumn<int>("splitType", splitType, "splitting classification", nanoaod::FlatTable::IntColumn);
    }

    iEvent.put(std::move(table));
}

DEFINE_FWK_MODULE(ShowerTreeInfoTableProducer);
