#include "SRothman/CustomJets/src/ParticleUncertainty.h"
#include "SRothman/Matching/src/simon_util_cpp/util.h"
#include <iostream>

void addUncertaintyToNeutralEM(particle& part){
    double E = part.pt * std::cosh(part.eta);
    part.dpt = std::sqrt( square(0.021/std::sqrt(E)) 
                        + square(0.094/E)
                        + square(0.005)) * part.pt;
    part.dphi = 0.0175;
    part.deta = part.dphi;
}

void addUncertaintyToNeutralHadron(particle& part){
    double E = part.pt * std::cosh(part.eta);
    part.dpt = std::sqrt( square(0.45/std::sqrt(E)) 
                        + square(0.05)) * part.pt;
    part.dphi = 0.022;
    part.deta = part.dphi;
}

void addUncertaintyToCharged(particle & part){
    part.dpt = std::sqrt( square(0.00025 * part.pt) 
                        + square(0.015)) * part.pt;
    part.dphi = std::sqrt( square(0.003)/part.pt) 
                         + square(0.0001);
    part.deta = part.dphi;
}

void addUncertainty(particle& part, bool inJetCore, double hardPt){
    if(part.pdgid==211 || part.pdgid==11 || part.pdgid==13){//charged particles
        if((part.pt < hardPt && !inJetCore) || part.pdgid == 13){ //muons get exemption
            addUncertaintyToCharged(part);
        } else {
            if(part.pdgid == 11){
                addUncertaintyToNeutralEM(part);
            } else {
                addUncertaintyToNeutralHadron(part);
            }
        }
    } else if (part.pdgid==22){//photons
        addUncertaintyToNeutralEM(part);
    } else if(part.pdgid==130){//neutral hadrons
        addUncertaintyToNeutralHadron(part);
    } else {
        std::cout << "Warning: unexpected pdgid " << part.pdgid << std::endl;
        addUncertaintyToNeutralHadron(part);
    }
}
