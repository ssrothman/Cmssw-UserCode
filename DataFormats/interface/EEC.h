#ifndef SROTHMAN_DATAFORMATS_EEC_H
#define SROTHMAN_DATAFORMATS_EEC_H

struct EECresult{
    unsigned iJet; 
    unsigned maxOrder;
    std::vector<size_t> offsets;
    std::vector<int> order;
    std::vector<double> wts;
    std::vector<double> dRs; 

    arma::mat cov;

    std::vector<double> res3wts;
    std::vector<double> res3dR1;
    std::vector<double> res3dR2;
    std::vector<double> res3dR3;

    arma::mat covRes3Res3;
    arma::mat covRes3Proj;

    std::vector<double> res4wts;
    std::vector<double> res4dR1;
    std::vector<double> res4dR2;
    std::vector<double> res4dR3;
    std::vector<double> res4dR4;
    std::vector<double> res4dR5;
    std::vector<double> res4dR6;

    arma::mat covRes4Res4;
    arma::mat covRes4Res3;
    arma::mat covRes4Proj;

};

#endif
