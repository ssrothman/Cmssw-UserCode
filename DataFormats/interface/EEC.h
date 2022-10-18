#ifndef DataFormats_EEC_H
#define DataFormats_EEC_H

#include <vector>
#include <memory>

//indexed by power, iPart, iDR
using coefs_t = std::vector<std::vector<std::vector<double>>>;

struct ProjectedEEC{
  int iJet;

  std::shared_ptr<std::vector<double>> dRvec;
  std::shared_ptr<std::vector<double>> wtvec;

  int order;

  std::shared_ptr<coefs_t> coefs;

  explicit ProjectedEEC(int jet, 
      std::shared_ptr<std::vector<double>>&& dR, 
      std::shared_ptr<std::vector<double>>&& wt, 
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

  std::shared_ptr<std::vector<std::vector<double>>> dRvec;
  std::shared_ptr<std::vector<double>> wtvec;

  int order;

  explicit ResolvedEEC(int jet, 
      std::shared_ptr<std::vector<std::vector<double>>>&& dR, 
      std::shared_ptr<std::vector<double>>&& wt, 
      int N):
    iJet(jet), 
    dRvec(std::move(dR)), wtvec(std::move(wt)), 
    order(N) {}
  ResolvedEEC() : iJet(-1), dRvec(nullptr), wtvec(nullptr), order(-1) {}
};

struct EECTransfer{
  std::shared_ptr<std::vector<double>> dRgen, dRreco;
  std::shared_ptr<std::vector<std::vector<double>>> matrix;

  explicit EECTransfer(std::shared_ptr<std::vector<double>> gen,
                       std::shared_ptr<std::vector<double>> reco,
                       std::shared_ptr<std::vector<std::vector<double>>> mat) :
    dRgen(gen), dRreco(reco), matrix(mat) {}
  EECTransfer() : dRgen(nullptr), dRreco(nullptr), matrix(nullptr) {}
};

typedef std::vector<ProjectedEEC> ProjectedEECCollection;
typedef std::vector<ResolvedEEC> ResolvedEECCollection;
typedef std::vector<EECTransfer> EECTransferCollection;

#endif
