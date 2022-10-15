#ifndef DataFormats_EMDFlow_H
#define DataFormats_EMDFlow_H

#include <vector>

struct EMDFlow{
  int iGen, iReco;
  std::vector<float> flow;

  explicit EMDFlow(int gen, int reco, std::vector<float> flowvec):
    iGen(gen), iReco(reco), flow(flowvec) {}
  EMDFlow() : iGen(-1), iReco(-1), flow() {}
};

typedef std::vector<EMDFlow> EMDFlowCollection;

#endif
