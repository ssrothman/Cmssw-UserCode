#ifndef SROTHMAN_CUSTOMJETS_ADDPARTICLE_H
#define SROTHMAN_CUSTOMJETS_ADDPARTICLE_H

#include "SRothman/SimonTools/src/jets.h"
#include "SRothman/SimonTools/src/isID.h"
#include "SRothman/SimonTools/src/selectionStructs.h"

template <typename P>
void addParticle(const P* const partptr, jet& ans, double jecfactor,
                 bool applyPuppi, bool applyJEC, 
                 bool onlyCharged,
                 double maxPartEta, 
                 const struct particleThresholds& thresholds,
                 const struct vtxCuts& vtxcuts,
                 unsigned maxNumPart){     

    int fromPV;
    double puppiWeight;
    if constexpr (std::is_same_v<P, pat::PackedCandidate>){
        fromPV = partptr->fromPV();
        puppiWeight = partptr->puppiWeight();
    } else {
        fromPV = true;
        puppiWeight = 1.0;
    }

    unsigned pdgid = std::abs(partptr->pdgId());
    if(pdgid == 12 || pdgid == 14 || pdgid == 16){//skip neutrinos
        return;
    }
    
    double nextpt = partptr->pt();

    if(applyPuppi){
        nextpt *= puppiWeight;
    }

    if(applyJEC){
        nextpt /= jecfactor;
    }

    double minPartPt = thresholds.getThreshold(partptr);
    if(nextpt < minPartPt 
            || ans.nPart >= maxNumPart 
            || nextpt==0
            || !vtxcuts.pass(partptr)
            || (onlyCharged && partptr->charge()==0)
            || std::abs(partptr->eta()) > maxPartEta){
        return;
    }

    ans.sumpt += nextpt;

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
