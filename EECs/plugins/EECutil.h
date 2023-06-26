#ifndef SROTHMAN_EECS_EECUTIL_H
#define SROTHMAN_EECS_EECUTIL_H

#include "SRothman/DataFormats/interface/jets.h"
#include "SRothman/DataFormats/interface/matching.h"
#include "SRothman/DataFormats/interface/EEC.h"

#include "SRothman/EECs/src/eec_oo.h"

#include <iostream>
#include <memory>
#include <vector>

template <bool nonIRC, bool doPU>
void addProjectedWTs(EECresult& result, 
                     const EECCalculator<PROJECTED, nonIRC, doPU>& calc,
                     const unsigned& order){
    result.offsets.emplace_back(result.wts.size());

    if constexpr (nonIRC){
        result.order.push_back(-(10*calc.getP1() + calc.getP2()));
    } else {
        result.order.push_back(order);
    }

    const std::vector<double>& wts = calc.getwts(order);
    result.wts.insert(result.wts.end(), wts.begin(), wts.end());
}

template <bool nonIRC, bool doPU>
void addProjectedDRs(EECresult& result,
            const EECCalculator<PROJECTED, nonIRC, doPU>& calc){
   const std::vector<double>& dRs = calc.getdRs();
   result.dRs.insert(result.dRs.end(), dRs.begin(), dRs.end());
}

template <bool nonIRC, bool doPU>
void addCovP(arma::mat& covp,
             const EECCalculator<PROJECTED, nonIRC, doPU>& calc,
             const unsigned& order,
             const size_t offset,
             int verbose=0){

    double accu=0;
    const arma::mat& cov = calc.getCov(order);
    for(unsigned i=0; i<cov.n_rows; ++i){
        for(unsigned j=0; j<cov.n_cols; ++j){
            covp(i + offset, j) = cov(i, j);
            accu += square(cov(i,j));
        }
    }
    if(verbose){
        printf("accumulated %0.3f in addCovP\n", accu);
    }

}

template <bool nonIRC, bool doPU>
void addResolved3(EECresult& result,
                     const EECCalculator<RESOLVED, nonIRC, doPU>& calc){
    result.res3dR1 = calc.getResolvedDRs(3, 0);
    result.res3dR2 = calc.getResolvedDRs(3, 1);
    result.res3dR3 = calc.getResolvedDRs(3, 2);
    result.res3wts = calc.getwts(3);
}

template <bool nonIRC, bool doPU>
void addResolved4(EECresult& result,
                     const EECCalculator<RESOLVED, nonIRC, doPU>& calc){
    result.res4dR1 = calc.getResolvedDRs(4, 0);
    result.res4dR2 = calc.getResolvedDRs(4, 1);
    result.res4dR3 = calc.getResolvedDRs(4, 2);
    result.res4dR4 = calc.getResolvedDRs(4, 3);
    result.res4dR5 = calc.getResolvedDRs(4, 4);
    result.res4dR6 = calc.getResolvedDRs(4, 5);
    result.res4wts = calc.getwts(4);
}

template <bool doPU>
void addEverything(EECresult& result,
    const EECCalculator<PROJECTED, false, false>& projcalc,
    const std::vector<EECCalculator<PROJECTED, true, doPU>>& nirccalcs,
    const EECCalculator<RESOLVED, false, false>& rescalc,
    unsigned iJet, unsigned nPart,
    int verbose=0){

    result.maxOrder = projcalc.getMaxOrder();
    result.iJet = iJet;

    addProjectedDRs(result, projcalc);

    for(unsigned order=2; order<=projcalc.getMaxOrder(); ++order){
        addProjectedWTs(result, projcalc, order);
    }

    for(const auto& calc : nirccalcs){
        addProjectedWTs(result, calc, 2u);
    }


    arma::mat covp(result.wts.size(), nPart, arma::fill::zeros);
    for(unsigned order=2; order<=projcalc.getMaxOrder(); ++order){
        if(verbose){
            printf("doing order %u\n", order);
        }
        addCovP(covp, projcalc, order, result.offsets[order-2], verbose);
        if(verbose){
            printf("after covp, sum is %0.3f\n", arma::dot(covp, covp));
        }
    }

    unsigned ioff = projcalc.getMaxOrder()+1;
    for(const auto& calc : nirccalcs){
        if(verbose){
            printf("doing nirccalc\n");
        }
        addCovP(covp, calc, 2, result.offsets[ioff-2], verbose);
        if(verbose){
            printf("after covp, sum is %0.3f\n", arma::dot(covp, covp));
        }
        ++ioff;
    }
    result.cov = covp * arma::trans(covp);
    if(verbose){
        printf("-------SUM COVPxP in EEC Producer is %0.3f------\n", arma::accu(result.cov));

        printf("about to call rescalc\n");
    }
    if(rescalc.hasRun()){
        if(verbose){
            printf("inside if\n");
        }
        addResolved3(result, rescalc);
        arma::mat covp3 = rescalc.getCov(3);
        result.covRes3Res3 = covp3 * arma::trans(covp3);
        result.covRes3Proj = covp3 * arma::trans(covp);
        if(rescalc.getMaxOrder() > 3){
            addResolved4(result, rescalc);
            arma::mat covp4 = rescalc.getCov(4);

            result.covRes4Res4 = covp4 * arma::trans(covp4);
            result.covRes4Res3 = covp4 * arma::trans(covp3);
            result.covRes4Proj = covp4 * arma::trans(covp);
        }
    }
    if(verbose){
        printf("after if\n");
    }

}


#endif
