#ifndef SROTHMAN_CUSTOMJETS_ADDPARTICLE_H
#define SROTHMAN_CUSTOMJETS_ADDPARTICLE_H

#include "SRothman/SimonTools/src/jets.h"
#include "SRothman/SimonTools/src/isID.h"
#include "SRothman/SimonTools/src/selectionStructs.h"
#include "SRothman/SimonTools/src/partSyst.h"
#include "SRothman/SimonTools/src/printPart.h"

template <typename P>
void addParticle(const P* const partptr, jet& ans, double jecfactor,
                 bool applyPuppi, bool applyJEC, 
                 bool onlyCharged,
                 double maxPartEta, 
                 const struct particleThresholds& thresholds,
                 const struct vtxCuts& vtxcuts,
                 partSyst& systematitics,
                 partSyst::SYSTEMATIC syst,
                 unsigned maxNumPart){     

    int fromPV;
    double puppiWeight, dxy, dz;
    if constexpr (std::is_same_v<P, pat::PackedCandidate>){
        fromPV = partptr->fromPV();
        puppiWeight = partptr->puppiWeight();
        dxy = partptr->dxy();
        dz = partptr->dz();
    } else {
        fromPV = true;
        puppiWeight = 1.0;
        dxy = 0;
        dz = 0;
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

    if(std::abs(partptr->eta()) > maxPartEta 
            || !vtxcuts.pass(partptr)
            || nextpt<=0){
        return;
    }

    particle nextpart(nextpt, partptr->eta(), partptr->phi(),
                      pdgid, partptr->charge(),
                      partptr->vertex().x(), 
                      partptr->vertex().y(), 
                      partptr->vertex().z(),
                      dxy, dz,
                      fromPV, puppiWeight);

    //printf("BEFORE SYSTEMATICS\n");
    //printPart(nextpart);
    if(!systematitics.applySystematic(syst, nextpart, ans.eta, ans.phi)){
        //printf("DROPPED BY SYSTEMATICS\n");
        return;
    }
    //printf("AFTER SYSTEMATICS\n");
    //printPart(nextpart);

    ans.rawpt += nextpart.pt;

    if(onlyCharged && nextpart.charge == 0){
        //printf("FAILED ONLYCHARGED\n");
        return;
    }

    //printf("\tadding particle with pt: %f, eta: %f, phi: %f, pdgid: %d\n", partptr->pt(), partptr->eta(), partptr->phi(), partptr->pdgId());
    double minPartPt = thresholds.getThreshold(nextpart);
    if(nextpart.pt < minPartPt 
            || ans.nPart >= maxNumPart 
            || !vtxcuts.pass(nextpart)){
        //printf("FAILED THRESHOLD or MAXNUM or VTX\n");
        return;
    }
    //printf("passed everything\n\n");

    ans.sumpt += nextpart.pt;

    ans.particles.push_back(nextpart);

    ++ans.nPart;
    if(isELE(nextpart)){
        ++ans.nELE;
    } else if (isMU(nextpart)){
        ++ans.nMU;
    } else if (isEM0(nextpart)){
        ++ans.nEM0;
    } else if (isHADCH(nextpart)){
        ++ans.nHADCH;
    } else if (isHAD0(nextpart)){
        ++ans.nHAD0;
    } else {
        printf("pdgid: %u charge: %d\n", pdgid, partptr->charge());
        printf("pt: %f eta: %f phi: %f\n", partptr->pt(), partptr->eta(), partptr->phi());
        throw std::logic_error("SimonJetProducer: unknown pdgid");
    }
}

#endif
