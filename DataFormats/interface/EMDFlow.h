#ifndef DataFormats_EMDFlow_H
#define DataFormats_EMDFlow_H

#include <vector>
#include <memory>
#include "SRothman/armadillo/include/armadillo"

using vecptr_t = std::shared_ptr<std::vector<double>>;

struct EMDFlow{
  int iGen, iReco;
  std::shared_ptr<arma::mat> flowmat;
  size_t Ngen, Nreco;
  vecptr_t EG, ER;

  explicit EMDFlow(int gen, int reco, std::shared_ptr<arma::mat>&& flow, 
      size_t NPGen, size_t NPReco, 
      vecptr_t&& EGv, vecptr_t&& ERv):
    iGen(gen), iReco(reco), flowmat(std::move(flow)), 
    Ngen(NPGen), Nreco(NPReco), 
    EG(std::move(EGv)), ER(std::move(ERv)) {}
  EMDFlow() : iGen(-1), iReco(-1), flowmat(nullptr), Ngen(0), Nreco(0), EG(nullptr), ER(nullptr) {}

  //use template to specialize for any potential input types
  //avoids any annoying compiler errors about signedness
  //and avoids any innefficient casts
  template <typename T>
  inline T idx(T iPGen, T iPReco){
    return iPGen*Nreco + iPReco;
  }

  template <typename T>
  double& at(T iPGen, T iPReco){
    return (*flowmat)(iPReco, iPGen);
  }
};

typedef std::vector<EMDFlow> EMDFlowCollection;

#endif
