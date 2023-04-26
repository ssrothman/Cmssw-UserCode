#include "SRothman/Matching/src/toyjets/common.h"

void addUncertainty(particle& part){
    if(part.pdgid==211){//charged hadrons
        part.dpt = 0.01 * part.pt;
        part.dphi = 0.05;
        part.deta = 0.05;
    } else if (part.pdgid==11){//electrons
        part.dpt = 0.01 * part.pt;
        part.dphi = 0.05;
        part.deta = 0.05;
    } else if (part.pdgid==13){//muons
        part.dpt = 0.01*part.pt;
        part.dphi = 0.05;
        part.deta = 0.05;
    } else if (part.pdgid==22){//photons
        part.dpt = 0.03*part.pt;
        part.dphi = 0.05;
        part.deta = 0.05;
    } else if(part.pdgid==130){//neutral hadrons
        part.dpt = 0.03*part.pt;
        part.dphi = 0.10;
        part.deta = 0.10;
    } else {//misc?
        part.dpt = 0.05*part.pt;
        part.dphi = 0.15;
        part.deta = 0.15;
    }
}
