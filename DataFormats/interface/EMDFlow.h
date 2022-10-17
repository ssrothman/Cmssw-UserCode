#ifndef DataFormats_EMDFlow_H
#define DataFormats_EMDFlow_H

#include <vector>
#include <memory>

struct EMDFlow{
  int iGen, iReco;
  std::shared_ptr<std::vector<float>> flowvec;
  size_t Ngen, Nreco;

  explicit EMDFlow(int gen, int reco, std::shared_ptr<std::vector<float>>&& flow, 
      size_t NPGen, size_t NPReco):
    iGen(gen), iReco(reco), flowvec(std::move(flow)), Ngen(NPGen), Nreco(NPReco) {}
  EMDFlow() : iGen(-1), iReco(-1), flowvec(nullptr), Ngen(0), Nreco(0) {}

  //use template to specialize for any potential input types
  //avoids any annoying compiler errors about signedness
  //and avoids any innefficient casts
  template <typename T>
  inline T idx(T iPGen, T iPReco){
    return iPReco*Ngen + iPGen;
  }

  template <typename T>
  float& at(T iPGen, T iPReco){
    return flowvec->at(idx(iPGen, iPReco));
  }
};

typedef std::vector<EMDFlow> EMDFlowCollection;

#endif
