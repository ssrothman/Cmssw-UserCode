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
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "SRothman/SimonTools/src/lund2.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>

struct ShowerTreeNode {
    reco::GenParticle const *  genPart;

    std::vector<ShowerTreeNode*> mothers;
    std::vector<ShowerTreeNode*> daughters;

    ShowerTreeNode(const reco::GenParticle * const genPart) : 
        genPart(genPart),
        mothers(), daughters() {}
};

class GenShowerSplittingsProducer : public edm::stream::EDProducer<> {
public:
    explicit GenShowerSplittingsProducer(const edm::ParameterSet&);
    ~GenShowerSplittingsProducer() override {}

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    void produce(edm::Event&, const edm::EventSetup&) override;

    void buildShowerTree(std::vector<ShowerTreeNode>& nodes, const edm::View<reco::GenParticle>& genParticles);

    int findSameParticle(const edm::View<reco::GenParticle>& genParticles, const reco::GenParticle* target);

    int findJetInitiator(const std::vector<ShowerTreeNode>& nodes, const reco::GenJet& jet);

    edm::InputTag genParticlesTag_;
    edm::EDGetTokenT<edm::View<reco::GenParticle>> genParticlesToken_;

    edm::InputTag genJetsTag_;
    edm::EDGetTokenT<edm::View<reco::GenJet>> genJetsToken_;
    
    bool hardSide_;

    int verbose_;
};

GenShowerSplittingsProducer::GenShowerSplittingsProducer(const edm::ParameterSet& iConfig) :
    genParticlesTag_(iConfig.getParameter<edm::InputTag>("src")),
    genParticlesToken_(consumes<edm::View<reco::GenParticle>>(genParticlesTag_)),
    genJetsTag_(iConfig.getParameter<edm::InputTag>("jets")),
    genJetsToken_(consumes<edm::View<reco::GenJet>>(genJetsTag_)),
    hardSide_(iConfig.getParameter<bool>("hardSide")),
    verbose_(iConfig.getParameter<int>("verbose"))
{
    produces<std::vector<simon::DoubleSplittingInfo>>();
}

void GenShowerSplittingsProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("src")->setComment("GenParticles collection");
    desc.add<edm::InputTag>("jets")->setComment("GenJets collection");
    desc.add<int>("verbose", 0)->setComment("Verbosity level");
    desc.add<bool>("hardSide", true)->setComment("Whether to follow the hard side of the splitting");
    descriptions.addWithDefaultLabel(desc);
}

int GenShowerSplittingsProducer::findJetInitiator(const std::vector<ShowerTreeNode>& nodes, const reco::GenJet& jet) {
    int hardest = -1;
    double hardest_pt = 0;
    for (size_t i = 0; i < nodes.size(); ++i){
        const auto& pi = nodes[i].genPart;

        if (pi->status()==1){ //reject final-state particles
            continue;
        }
        if (pi->status()==2) { // reject hadrons, muons and taus
            continue;
        }
        if (pi->status()==4) { //reject beam particles
            continue;
        }
        if (pi->pdgId() != 21 && (
            std::abs(pi->pdgId()) < 1 || std::abs(pi->pdgId()) > 6)){ // reject non-quarks/gluons
            
                continue;
        }
        if (reco::deltaR(pi->eta(), pi->phi(), jet.eta(), jet.phi()) > 0.4){ // reject particles too far away from the jet
            continue;
        }

        if (pi->pt() > hardest_pt){
            hardest = i;
            hardest_pt = pi->pt();
        }
    }

    if (hardest < 0){
        return -1;
    }

    return hardest;
}

int GenShowerSplittingsProducer::findSameParticle(const edm::View<reco::GenParticle>& genParticles, const reco::GenParticle* target) {
    for (size_t i = 0; i < genParticles.size(); ++i) {
        if (&(genParticles.at(i)) == target) {
            return i;
        }
    }
    return -1; // Not found
}

void GenShowerSplittingsProducer::buildShowerTree(std::vector<ShowerTreeNode>& nodes, const edm::View<reco::GenParticle>& genParticles) {
    nodes.clear();
    nodes.reserve(genParticles.size());
    for (const auto& genPart : genParticles) { 
        ShowerTreeNode node(&genPart);
        nodes.push_back(node);
    }

    // Now populate the mother and daughter pointers
    for (size_t i = 0; i < genParticles.size(); ++i) {
        const auto& genPart = genParticles.at(i);
        ShowerTreeNode& node = nodes[i];

        // Mothers
        for (size_t m = 0; m < genPart.numberOfMothers(); ++m) {
            const reco::GenParticle* mother = static_cast<const reco::GenParticle*>(genPart.mother(m));
            int motherIndex = findSameParticle(genParticles, mother);
            if (motherIndex >= 0) {
                node.mothers.push_back(&nodes[motherIndex]);
            } else {
                throw std::runtime_error("Mother particle not found in genParticles collection");
            }
        }
        //sort mothers by pt, highest first
        std::sort(node.mothers.begin(), node.mothers.end(), [](const ShowerTreeNode* a, const ShowerTreeNode* b){
            return a->genPart->pt() > b->genPart->pt();
        });
        
        // Daughters
        for (size_t d = 0; d < genPart.numberOfDaughters(); ++d) {
            const reco::GenParticle* daughter = static_cast<const reco::GenParticle*>(genPart.daughter(d));
            int daughterIndex = findSameParticle(genParticles, daughter);
            if (daughterIndex >= 0) {
                node.daughters.push_back(&nodes[daughterIndex]);
            } else {
                throw std::runtime_error("Daughter particle not found in genParticles collection");
            }
        }
        //sort daughters by pt, highest first
        std::sort(node.daughters.begin(), node.daughters.end(), [](const ShowerTreeNode* a, const ShowerTreeNode* b){
            return a->genPart->pt() > b->genPart->pt();
        });
    }
}

void GenShowerSplittingsProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {   
    edm::Handle<edm::View<reco::GenParticle>> genParticles;
    iEvent.getByToken(genParticlesToken_, genParticles);
    
    edm::Handle<edm::View<reco::GenJet>> genJets;
    iEvent.getByToken(genJetsToken_, genJets);

    auto splittinginfo = std::make_unique<std::vector<simon::DoubleSplittingInfo>>();

    // Build the shower tree information for each particle
    if (genJets->size() > 0){
        std::vector<ShowerTreeNode> genTree;
        buildShowerTree(genTree, *genParticles);
        if (verbose_ > 0){
            printf("Built shower tree with %lu nodes\n", genTree.size());
        }
        
        const auto& j0 = genJets->at(0);

        if (verbose_ > 0){
            printf("The hardest jet has (pt, eta, phi) = (%g, %f, %f)\n",
            j0.pt(), j0.eta(), j0.phi());
        }

        int initiator = findJetInitiator(genTree, j0);
        if (initiator >= 0){
       
            ShowerTreeNode& start = genTree[initiator];
            while (start.daughters.size() == 1 && start.genPart->pdgId() == start.daughters[0]->genPart->pdgId()){
                start = *(start.daughters[0]);
            }

            if (verbose_ > 0){
                printf("Found an initial-state particle near the jet with (pt, eta, phi) = (%g, %f, %f)\n",
                    start.genPart->pt(), start.genPart->eta(), start.genPart->phi());
                printf("\tand pdgid, charge = (%d, %d)\n",
                    start.genPart->pdgId(), start.genPart->charge());
            }

            if (start.daughters.size() != 2){
                if (verbose_ > 1){
                    printf("The initial splitting has %lu daughters; skipping\n", start.daughters.size());
                }
            } else {
                const ShowerTreeNode* d1;
                
                if (hardSide_){
                    d1 = start.daughters[0];
                } else {
                    d1 = start.daughters[1];
                }
                
                while (d1->daughters.size() == 1 && d1->genPart->pdgId() == d1->daughters[0]->genPart->pdgId()){
                    d1 = d1->daughters[0];
                }

                if (d1->daughters.size() != 2){
                    if (verbose_ > 1){
                        printf("The second splitting has %lu daughters; skipping\n", d1->daughters.size());
                    }
                } else {
                    simon::DoubleSplittingInfo info(
                        *start.genPart,
                        *start.daughters[0]->genPart,
                        *start.daughters[1]->genPart,
                        *d1->genPart,
                        *d1->daughters[0]->genPart,
                        *d1->daughters[1]->genPart
                    );
                    if (verbose_ > 0){
                        printf("First splitting is (%d) -> (%d) (%d)\n",
                            info.split123.pdgId1, info.split123.pdgId2, info.split123.pdgId3
                        );
                        printf("Second splitting is (%d) -> (%d) (%d)\n",
                            info.split456.pdgId1, info.split456.pdgId2, info.split456.pdgId3
                        );
                    }
                    splittinginfo->push_back(info);
                }
            }
        }
    }
    
    iEvent.put(std::move(splittinginfo));
}

DEFINE_FWK_MODULE(GenShowerSplittingsProducer);