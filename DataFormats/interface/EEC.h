#ifndef DataFormats_EEC_H
#define DataFormats_EEC_H

#include <vector>
#include <memory>

using coefs_t = std::vector<std::vector<std::vector<float>>>;

struct ProjectedEEC{
  int iJet;

  std::shared_ptr<std::vector<float>> dRvec;
  std::shared_ptr<std::vector<float>> wtvec;

  int order;

  std::shared_ptr<coefs_t> coefs;

  explicit ProjectedEEC(int jet, 
      std::shared_ptr<std::vector<float>>&& dR, 
      std::shared_ptr<std::vector<float>>&& wt, 
      int N,
      std::shared_ptr<coefs_t>&& coeficients):
    iJet(jet), 
    dRvec(std::move(dR)), wtvec(std::move(wt)), 
    order(N), 
    coefs(std::move(coeficients)) {}
  ProjectedEEC() : iJet(-1), dRvec(nullptr), wtvec(nullptr), order(-1), coefs(nullptr) {}
};

struct ResolvedEEC{
  int iJet;

  std::shared_ptr<std::vector<std::vector<float>>> dRvec;
  std::shared_ptr<std::vector<float>> wtvec;

  int order;

  explicit ResolvedEEC(int jet, 
      std::shared_ptr<std::vector<std::vector<float>>>&& dR, 
      std::shared_ptr<std::vector<float>>&& wt, 
      int N):
    iJet(jet), 
    dRvec(std::move(dR)), wtvec(std::move(wt)), 
    order(N) {}
  ResolvedEEC() : iJet(-1), dRvec(nullptr), wtvec(nullptr), order(-1) {}
};

typedef std::vector<ProjectedEEC> ProjectedEECCollection;
typedef std::vector<ResolvedEEC> ResolvedEECCollection;

#endif