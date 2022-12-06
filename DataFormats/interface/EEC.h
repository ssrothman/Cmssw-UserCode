#ifndef DataFormats_EEC_H
#define DataFormats_EEC_H

#include <vector>
#include <memory>

#include "SRothman/armadillo/include/armadillo"
#include "SRothman/EECs/src/vecND.h"

//indexed by power, iPart, iDR
using coefs_t = std::vector<std::vector<std::vector<double>>>;

//pointers to vectors
using vecptr_d_t = std::shared_ptr<std::vector<double>>;
using vecptr_i_t = std::shared_ptr<std::vector<int>>;
using vecptr_b_t = std::shared_ptr<std::vector<bool>>;

struct EECParts{
  int iJet;

  double rawPt;      //sum of all jet consituent pt (including those with pt<minPartPt)
  double minPartPt;  //pt threshold

  vecptr_d_t partPt;
  vecptr_d_t partEta;
  vecptr_d_t partPhi;
  vecptr_i_t partPdgId;

  explicit EECParts(int iJet, double rawPt, double minPartPt,
                    vecptr_d_t&& partPt, vecptr_d_t&& partEta, vecptr_d_t&& partPhi,
                    vecptr_i_t&& partPdgId):
    iJet(iJet), rawPt(rawPt), minPartPt(minPartPt),
    partPt(std::move(partPt)), partEta(std::move(partEta)), partPhi(std::move(partPhi)),
    partPdgId(std::move(partPdgId)) {}
    
  EECParts():
    iJet(-1), rawPt(-1), minPartPt(-1),
    partPt(nullptr), partEta(nullptr), partPhi(nullptr),
    partPdgId(nullptr) {}
};
    

struct ProjectedEEC{
  int iJet;

  vecptr_d_t dRvec;
  vecptr_d_t wtvec;

  int order;

  std::shared_ptr<coefs_t> coefs;
  std::shared_ptr<vecND<double>> tuplewts;
  std::shared_ptr<vecND<int>> tupleiDR;

  explicit ProjectedEEC(int jet, 
      vecptr_d_t&& dR, 
      vecptr_d_t&& wt, 
      int N,
      std::shared_ptr<coefs_t>&& coeficients,
      std::shared_ptr<vecND<double>>&& tuplewts,
      std::shared_ptr<vecND<int>>&& tupleiDR):
    iJet(jet), 
    dRvec(std::move(dR)), wtvec(std::move(wt)), 
    order(N), 
    coefs(std::move(coeficients)), 
    tuplewts(std::move(tuplewts)), 
    tupleiDR(std::move(tupleiDR))
  {}
  ProjectedEEC() : 
    iJet(-1), 
    dRvec(nullptr), wtvec(nullptr), 
    order(-1), 
    coefs(nullptr), tuplewts(nullptr), tupleiDR(nullptr) {}
};

struct ResolvedEEC{
  int iJet;

  std::shared_ptr<std::vector<std::vector<double>>> dRvec;
  vecptr_d_t wtvec;

  int order;

  explicit ResolvedEEC(int jet, 
      std::shared_ptr<std::vector<std::vector<double>>>&& dR, 
      vecptr_d_t&& wt, 
      int N):
    iJet(jet), 
    dRvec(std::move(dR)), wtvec(std::move(wt)), 
    order(N) {}
  ResolvedEEC() : iJet(-1), dRvec(nullptr), wtvec(nullptr), order(-1) {}
};

struct EECTransfer{
  int iJetGen, iJetReco;
  vecptr_d_t dRgen, dRreco, wtgen, wtreco;
  std::shared_ptr<arma::mat> matrix;

  explicit EECTransfer(int iGen, int iReco,
                       vecptr_d_t& genDR, vecptr_d_t& recoDR, 
                       vecptr_d_t& genWT, vecptr_d_t& recoWT,
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
typedef std::vector<EECParts> EECPartsCollection;

#endif
