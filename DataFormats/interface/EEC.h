#ifndef SROTHMAN_DATAFORMATS_EEC_H
#define SROTHMAN_DATAFORMATS_EEC_H

struct EECresult{
    unsigned iJet, iReco; 

    std::vector<int> orders;
    std::vector<std::vector<double>> wts;
    std::vector<arma::mat> covs;

    std::vector<double> res3wts;
    arma::mat cov3;

    std::vector<double> res4wts;
    arma::mat cov4;

    EECresult() : iJet(0), iReco(0), orders(), 
                  wts(), covs(), 
                  res3wts(), cov3(), 
                  res4wts(), cov4() {}
};

struct EECtransfer{
    unsigned iReco, iGen;
    std::vector<arma::mat> proj;
    std::vector<int> orders;
    arma::mat res3;
    arma::mat res4;
};

#endif
