#ifndef SROTHMAN_CUSTOMJETS_ADDPARTICLE_H
#define SROTHMAN_CUSTOMJETS_ADDPARTICLE_H

#include "SRothman/SimonTools/src/jets.h"
#include "SRothman/SimonTools/src/isID.h"

template <typename P>
void addParticle(const P* partptr, jet& ans, double jecfactor,
                 bool applyPuppi, bool applyJEC, 
                 double minPartPt, double maxPartEta,
                 unsigned maxNumPart){     
    int fromPV;
    double puppiWeight;
    if constexpr (std::is_same_v<P, pat::PackedCandidate>){
        fromPV = partptr->fromPV();
        puppiWeight = partptr->puppiWeight();
    } else {
        fromPV = (partptr->dxy()==0) && (partptr->dz()==0);
        puppiWeight = 1.0;
    }

    double nextpt = partptr->pt();
    if(applyPuppi){
        nextpt *= puppiWeight;
    }
    if(applyJEC){
        nextpt /= jecfactor;
    }
    ans.sumpt += nextpt;

    if(nextpt < minPartPt || ans.nPart >= maxNumPart || nextpt==0){
        return;
    }

    unsigned pdgid = std::abs(partptr->pdgId());
    if(pdgid == 12 || pdgid == 14 || pdgid == 16){//skip neutrinos
        return;
    }
    
    if(std::abs(partptr->eta()) > maxPartEta){
        return;
    }

    ans.particles.emplace_back(
            nextpt, partptr->eta(), partptr->phi(),
            pdgid, partptr->charge(),
            partptr->vertex().x(), 
            partptr->vertex().y(), 
            partptr->vertex().z(),
            partptr->dxy(), partptr->dz(),
            fromPV, puppiWeight
    );
    ++ans.nPart;
    if(isELE(partptr)){
        ++ans.nELE;
    } else if (isMU(partptr)){
        ++ans.nMU;
    } else if (isEM0(partptr)){
        ++ans.nEM0;
    } else if (isHADCH(partptr)){
        ++ans.nHADCH;
    } else if (isHAD0(partptr)){
        ++ans.nHAD0;
    } else {
        printf("pdgid: %u charge: %d\n", pdgid, partptr->charge());
        printf("pt: %f eta: %f phi: %f\n", partptr->pt(), partptr->eta(), partptr->phi());
        throw std::logic_error("SimonJetProducer: unknown pdgid");
    }
}

#endif