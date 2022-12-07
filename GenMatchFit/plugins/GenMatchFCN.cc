#include "SRothman/GenMatchFit/plugins/GenMatchFCN.h"
#include <cmath>
#include <iostream>

static double square(double val){
  return val*val;
}

double GenMatchFCN::operator()(const std::vector<double>& data) const{
  //A matrix
  std::vector<double> A(NPReco*NPGen, 0);
  
  //predicted pT, eta, phi
  std::vector<double> EP(NPReco, 0);
  
  //constraint functions
  std::vector<double> C(NPGen, 0);
  
  double mu;
  std::vector<double> lambdas(NPGen, 0);

  for(size_t iGen=0; iGen<NPGen; ++iGen){
    lambdas[iGen] = data.at(iGen);
  }
  mu = data.at(NPGen+1);

  //compute column sums
  for(size_t iReco=0; iReco<NPReco; ++iReco){
    for(size_t iGen=0; iGen<NPGen; ++iGen){
      C.at(iGen) += data.at(idx(iReco, iGen));
    }
  }

  //fill A matrix to have column sums = 1
  for(size_t iReco=0; iReco<NPReco; ++iReco){
    for(size_t iGen=0; iGen<NPGen; ++iGen){
      if(C.at(iGen)>0){
        A.at(idx(iReco, iGen)-NPGen-1) = data.at(idx(iReco, iGen))/C.at(iGen);
      } else {
        A.at(idx(iReco, iGen)-NPGen-1) = 0;
      }
    }
  }

  //calculate predicted pT
  for(size_t iReco=0; iReco<NPReco; ++iReco){
    for(size_t iGen=0; iGen<NPGen; ++iGen){
      EP.at(iReco) += data.at(idx(iReco, iGen)) * genPT->at(iGen);
    }
  }

  //calculate chi squared
  double chisq = 0;
  for(size_t iReco=0; iReco<NPReco; ++iReco){
    if (EP[iReco]>0){ //reco particle got matched
      //gaussian energy likelihood
      chisq += square( (EP.at(iReco) - recoPT->at(iReco))/errPT->at(iReco));
      for(size_t iGen=0; iGen<NPGen; ++iGen){
        //gaussian likelihoods for merging gen particle into reco particle
        double A = data.at(idx(iReco, iGen));
        if(A==0){
          continue;
        } else {
          chisq += A * square((genETA->at(iGen) - recoETA->at(iReco))/errETA->at(iReco));
          chisq += A * square((genPHI->at(iGen) - recoPHI->at(iReco))/errPHI->at(iReco));
        }
      }
    } else { //reco particle didn't get matched
      //power law likelihood
      //clips to zero so that low-energy particles aren't favorably marked as PU
      //extra penalty term to incentivize matching low-energy particles
      chisq += std::max(2*PUexp*std::log(recoPT->at(iReco)), 0.0) + PUpenalty;
    }
  }

  //augmented lagrangian constraint enforcement terms
  //L = f(x) + mu/2 sum_i c^2 + lambda_i c_i
  for(size_t iGen=0; iGen<NPGen; ++iGen){
    chisq += 0.5 * mu * square(C.at(iGen) - 1.0); //+ lambdas.at(iGen)*C.at(iGen);
  }

  return chisq;
};
