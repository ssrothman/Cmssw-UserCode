#ifndef SROTHMAN_DATAFORMATS_MATCHING_H
#define SROTHMAN_DATAFORMATS_MATCHING_H

#include <Eigen/Dense>

namespace matching {
    struct jetmatch { 
        unsigned iReco, iGen;
        Eigen::MatrixXd tmat;
    };
};

#endif
