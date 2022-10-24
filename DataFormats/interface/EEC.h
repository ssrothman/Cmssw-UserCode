#ifndef DataFormats_EEC_H
#define DataFormats_EEC_H

#include <vector>
#include <memory>

#include "SRothman/armadillo/include/armadillo"

//indexed by power, iPart, iDR
using coefs_t = std::vector<std::vector<std::vector<double>>>;
using vecptr_t = std::shared_ptr<std::vector<double>>;

struct ProjectedEEC{
  int iJet;

  vecptr_t dRvec;
  vecptr_t wtvec;

  int order;

  std::shared_ptr<coefs_t> coefs;

  explicit ProjectedEEC(int jet, 
      vecptr_t&& dR, 
      vecptr_t&& wt, 
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
  vecptr_t wtvec;

  int order;

  explicit ResolvedEEC(int jet, 
      std::shared_ptr<std::vector<std::vector<double>>>&& dR, 
      vecptr_t&& wt, 
      int N):
    iJet(jet), 
    dRvec(std::move(dR)), wtvec(std::move(wt)), 
    order(N) {}
  ResolvedEEC() : iJet(-1), dRvec(nullptr), wtvec(nullptr), order(-1) {}
};

struct EECTransfer{
  int iJetGen, iJetReco;
  vecptr_t dRgen, dRreco, wtgen, wtreco;
  std::shared_ptr<arma::mat> matrix;

  explicit EECTransfer(int iGen, int iReco,
                       vecptr_t& genDR, vecptr_t& recoDR, 
                       vecptr_t& genWT, vecptr_t& recoWT,
                       std::shared_ptr<arma::mat> mat) :
    iJetGen(iGen), iJetReco(iReco),
    dRgen(genDR), dRreco(recoDR), 
    wtgen(genWT), wtreco(recoWT), 
    matrix(mat) {}
  EECTransfer() : iJetGen(-1), iJetReco(-1),
                  dRgen(nullptr), dRreco(nullptr), 
                  wtgen(nullptr), wtreco(nullptr),  
                  matrix(nullptr) {}
};

typedef std::vector<ProjectedEEC> ProjectedEECCollection;
typedef std::vector<ResolvedEEC> ResolvedEECCollection;
typedef std::vector<EECTransfer> EECTransferCollection;

#endif
