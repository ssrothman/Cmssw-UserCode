#ifndef SROTHMAN_DATAFORMATS_JETS_H
#define SROTHMAN_DATAFORMATS_JETS_H

#include "SRothman/Matching/src/toyjets/common.h"
#include "SRothman/armadillo-12.2.0/include/armadillo"

inline arma::vec ptvec(const jet& j){
    arma::vec ans(j.nPart, arma::fill::none);
    for(unsigned i=0; i<j.nPart; ++i){
        ans(i) = j.particles[i].pt;
    }
    return ans;
}

#endif
