#ifndef SROTHMAN_DATAFORMATS_EEC_H
#define SROTHMAN_DATAFORMATS_EEC_H

struct EECresult{
    unsigned iJet, iReco; 

    std::vector<int> orders;
    std::vector<std::vector<double>> wts;

    std::vector<double> res3wts;

    std::vector<double> res4wts;

    EECresult() : iJet(0), iReco(0), orders(), 
                  wts(),
                  res3wts(), 
                  res4wts() {}
};

struct EECtransfer{
    unsigned iReco, iGen;
    std::vector<arma::mat> proj;
    std::vector<int> orders;
    arma::mat res3;
    arma::mat res4;
};

#endif
