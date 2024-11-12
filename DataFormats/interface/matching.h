#ifndef SROTHMAN_DATAFORMATS_MATCHING_H
#define SROTHMAN_DATAFORMATS_MATCHING_H

#include <Eigen/Dense>

struct jetmatch { 
    unsigned iReco, iGen;
    Eigen::MatrixXd ptrans;
    Eigen::MatrixXd rawmat;
};

#endif
