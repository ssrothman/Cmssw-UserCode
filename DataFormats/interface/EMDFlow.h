#ifndef DataFormats_EMDFlow_H
#define DataFormats_EMDFlow_H

#include <vector>
#include <memory>

struct EMDFlow{
  int iGen, iReco;
  std::shared_ptr<std::vector<float>> flowvec;

  explicit EMDFlow(int gen, int reco, std::shared_ptr<std::vector<float>>&& flow):
    iGen(gen), iReco(reco), flowvec(std::move(flow)) {}
  EMDFlow() : iGen(-1), iReco(-1), flowvec(nullptr) {}
};

typedef std::vector<EMDFlow> EMDFlowCollection;

#endif
