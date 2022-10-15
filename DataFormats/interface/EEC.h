#ifndef DataFormats_EMDFlow_H
#define DataFormats_EMDFlow_H

#include <vector>

struct ProjectedEEC{
  int iGen, iReco;
  int order;
  std::vector<float> dRvec;
  std::vector<float> wtvec;

  explicit ProjectedEEC(int gen, int reco, std::vector<float> dR, std::vector<float> wt, N):
    iGen(gen), iReco(reco), dRvec(dR), wtvec(wt), order(N) {}
  ProjectedEEC() : iGen(-1), iReco(-1), dRvec(), wtvec(), order(-1) {}
};

struct ResolvedEEC{
  int iGen, iReco;
  int order;
  std::vector<std::vector<float>> dRvec;
  std::vector<float> wtvec;

  explicit ResolvedEEC(int gen, int reco, std::vector<float> dR, std::vector<float> wt, N):
    iGen(gen), iReco(reco), dRvec(dR), wtvec(wt), order(N) {}
  ResolvedEEC() : iGen(-1), iReco(-1), dRvec(), wtvec(), order(-1) {}
};



typedef std::vector<ProjectedEEC> ProjectedEECCollection;
typedef std::vector<ResolvedEEC> ResolvedEECCollection;

#endif
