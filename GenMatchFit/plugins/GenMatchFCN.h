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
    std::vector<double> lambdas;
    double mu;

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
      PUexp(PUexp), PUpenalty(PUpenalty), 
      lambdas(NPGen, startLambda), mu(startMu) { }

    inline const size_t idx(const size_t iReco, const size_t iGen) const{
      return iReco*NPGen + iGen;
    }

    //NB updateLambdas() must be called before updateMu()
    double updateLambdas(const std::vector<double>& data){
      double feas=0;
      size_t N=0;

      std::vector<double> C(NPGen, 0);
      for(size_t iGen=0; iGen<NPGen; ++iGen){
        for(size_t iReco=0; iReco<NPReco; ++iReco){
          C.at(iGen) += data.at(idx(iReco, iGen));
        }
        if(C.at(iGen)==0){
          continue;
        }
        ++N;
        C.at(iGen)-=1;
        feas += C.at(iGen)*C.at(iGen);
        lambdas.at(iGen) += mu * C.at(iGen);
        printf("%lu: lambda = %0.3f\t C = %0.3f\n", iGen, lambdas.at(iGen), C.at(iGen));
      }

      return feas/N;
    }

    inline void updateMu(const double factor){
      mu *= factor;
      printf("mu = %0.2f\n", mu);
    }

    double operator()(const std::vector<double>& data) const override;

    //error computation constant
    //should be 1.0 for our chisq likelihood 
    inline double Up() const override {return 1.0;}
};

#endif
