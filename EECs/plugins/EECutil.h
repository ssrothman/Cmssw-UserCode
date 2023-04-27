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
    result.order.push_back(order);

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
             const size_t offset){

    const arma::mat& cov = calc.getCov(order);
    for(unsigned i=0; i<cov.n_rows; ++i){
        for(unsigned j=0; j<cov.n_cols; ++j){
            covp(i + offset, j) = cov(i, j);
        }
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

#endif
