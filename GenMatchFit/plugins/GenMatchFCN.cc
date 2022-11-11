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
  std::vector<double> EP(NPReco, 0), etaP(NPReco, 0), phiP(NPReco, 0);
  
  //constraint functions
  std::vector<double> C(NPGen, 0);
  
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
        A.at(idx(iReco, iGen)) = data.at(idx(iReco, iGen))/C.at(iGen);
      } else {
        A.at(idx(iReco, iGen)) = 0;
      }
    }
  }

  //calculate predicted pT
  for(size_t iReco=0; iReco<NPReco; ++iReco){
    for(size_t iGen=0; iGen<NPGen; ++iGen){
      EP.at(iReco) += data.at(idx(iReco, iGen)) * genPT->at(iGen);
    }
  }

  //calculate predicted eta, phi
  for(size_t iReco=0; iReco<NPReco; ++iReco){
    if(EP.at(iReco) == 0){//skip unmatched particles
      continue;
    }
    for(size_t iGen=0; iGen<NPGen; ++iGen){
      double Z = data.at(idx(iReco, iGen)) * genPT->at(iGen) / EP.at(iReco);
      etaP[iReco] += Z * genETA->at(iGen);
      phiP[iReco] += Z * genPHI->at(iGen);
    }
  }

  //calculate chi squared
  double chisq = 0;
  for(size_t iReco=0; iReco<NPReco; ++iReco){
    if (EP[iReco]>0){ //reco particle got matched
      //add gaussian likelihood terms to chi squared
      chisq += square( (EP.at(iReco) - recoPT->at(iReco))/errPT->at(iReco));
      chisq += square( (etaP.at(iReco) - recoETA->at(iReco))/errETA->at(iReco));
      chisq += square( (phiP.at(iReco) - recoPHI->at(iReco))/errPHI->at(iReco));
    } else { //reco particle didn't get matched
      //power law likelihood
      //clips to zero so that low-energy particles aren't favorably marked as PU
      //extra penalty term to incentivize matching low-energy particles
      chisq += std::max(2*PUexp*std::log(recoPT->at(iReco)), 0.0) + PUpenalty;
    }
  }

  //lagrange multiplier-style constraint functions for column sums
  for(size_t iGen=0; iGen<NPGen; ++iGen){
    C.at(iGen) -= 1;
  }

  //augmented lagrangian constraint enforcement terms
  //L = f(x) + mu/2 sum_i c^2 + lambda_i c_i
  for(size_t iGen=0; iGen<NPGen; ++iGen){
    chisq += 0.5 * mu * square(C.at(iGen)) + lambdas.at(iGen)*C.at(iGen);
  }

  return chisq;
};
