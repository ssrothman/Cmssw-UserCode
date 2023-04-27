#ifndef SROTHMAN_DATAFORMATS_MATCHING_H
#define SROTHMAN_DATAFORMATS_MATCHING_H

#include "SRothman/armadillo-12.2.0/include/armadillo"

struct jetmatch { 
    unsigned iReco, iGen;
    arma::mat ptrans;
};

#endif
