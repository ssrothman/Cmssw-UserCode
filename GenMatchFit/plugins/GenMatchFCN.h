#ifndef GENMATCHFCN_H
#define GENMATCHFN_H

#include "Minuit2/FCNBase.h"
#include <vector>
#include <memory>

using vecptr = std::shared_ptr<std::vector<double>>;
using cvecptr = const std::shared_ptr<const std::vector<double>>;

class GenMatchFCN: public ROOT::Minuit2::FCNBase {
  private:
    //data defining fit problem
    cvecptr recoPT, recoETA, recoPHI;
    cvecptr genPT, genETA, genPHI;
    cvecptr errPT, errETA, errPHI;

    const size_t NPReco, NPGen;
    const double PUexp, PUpenalty;

    //constants for augmented lagrangian method
    //will be used to enforce sum_i Aij = 1
  public:
    explicit GenMatchFCN(cvecptr recoPT, cvecptr recoETA, cvecptr recoPHI,
                         cvecptr genPT, cvecptr genETA, cvecptr genPHI,
                         cvecptr errPT, cvecptr errETA, cvecptr errPHI,
                         const double PUexp, const double PUpenalty,
                         const double startMu, const double startLambda):
      recoPT(recoPT), recoETA(recoETA), recoPHI(recoPHI),
      genPT(genPT), genETA(genETA), genPHI(genPHI),
      errPT(errPT), errETA(errETA), errPHI(errPHI),
      NPReco(recoPT->size()), NPGen(genPT->size()),
      PUexp(PUexp), PUpenalty(PUpenalty) {}

    inline const size_t idx(const size_t iReco, const size_t iGen) const{
      return iReco*NPGen + iGen + NPGen + 1;
    }

    double getFeas(const std::vector<double>& data){
      double feas=0;
      std::vector<double> C(NPGen,0);

      for(size_t iGen=0; iGen<NPGen; ++iGen){
        for(size_t iReco=0; iReco<NPReco; ++iReco){
          C.at(iGen) += data.at(idx(iReco, iGen));
        }
      }

      for(size_t iGen=0; iGen<NPGen; ++iGen){
        if(C[iGen]!=0){
          feas += (C[iGen]-1)*(C[iGen]-1);
        }
      }
      return feas;
    }

    double operator()(const std::vector<double>& data) const override;

    //error computation constant
    //should be 1.0 for our chisq likelihood 
    inline double Up() const override {return 1.0;}
};

#endif
