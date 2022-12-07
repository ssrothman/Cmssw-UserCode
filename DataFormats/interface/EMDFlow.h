#ifndef DataFormats_EMDFlow_H
#define DataFormats_EMDFlow_H

#include <vector>
#include <memory>
#include "SRothman/armadillo/include/armadillo"

using vecptr_d_t = std::shared_ptr<std::vector<double>>;
using vecptr_b_t = std::shared_ptr<std::vector<bool>>;

struct EMDFlow{
  int iGen, iReco;
  std::shared_ptr<arma::mat> flowmat;
  size_t Ngen, Nreco;
  vecptr_d_t EG, ER;
  vecptr_b_t matchedG, matchedR;

  explicit EMDFlow(int gen, int reco, std::shared_ptr<arma::mat>&& flow, 
      size_t NPGen, size_t NPReco, 
      vecptr_d_t&& EGv, vecptr_d_t&& ERv,
      vecptr_b_t&& matchedG, vecptr_b_t&& matchedR):
    iGen(gen), iReco(reco), flowmat(std::move(flow)), 
    Ngen(NPGen), Nreco(NPReco), 
    EG(std::move(EGv)), ER(std::move(ERv)),
    matchedG(std::move(matchedG)), matchedR(std::move(matchedR)){}
  EMDFlow() : 
    iGen(-1), iReco(-1), flowmat(nullptr), 
    Ngen(0), Nreco(0), 
    EG(nullptr), ER(nullptr),
    matchedG(nullptr), matchedR(nullptr){}

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
