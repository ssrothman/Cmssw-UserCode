#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "DataFormats/NanoAOD/interface/FlatTable.h"

#include "SRothman/DataFormats/interface/EEC.h"
#include "SRothman/EECs/src/iterating.h"

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cstdarg>

#define MAX_CONSTITUENTS 256

#define VERBOSE 

// requires at least C++11
//from https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/49812018#49812018
static const std::string vformat(const char * const zcFormat, ...) {

  // initialize use of the variable argument array
  va_list vaArgs;
  va_start(vaArgs, zcFormat);

  // reliably acquire the size
  // from a copy of the variable argument array
  // and a functionally reliable call to mock the formatting
  va_list vaArgsCopy;
  va_copy(vaArgsCopy, vaArgs);
  const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
  va_end(vaArgsCopy);

  // return a formatted string without risking memory mismanagement
  // and without assuming any compiler or platform specific behavior
  std::vector<char> zc(iLen + 1);
  std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
  va_end(vaArgs);
  return std::string(zc.data(), iLen); 
}

template <typename T, typename K>
class EECTableProducerT : public edm::stream::EDProducer<> {
public:
  explicit EECTableProducerT(const edm::ParameterSet&);
  ~EECTableProducerT() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  std::string name_;

  edm::InputTag jetsTag_;
  edm::EDGetTokenT<edm::View<T>> jetsToken_;

  edm::InputTag EECTag_;
  edm::EDGetTokenT<K> EECToken_;

  unsigned nDR_;

  bool doParts_;
};

template <typename T, typename K>
EECTableProducerT<T, K>::EECTableProducerT(const edm::ParameterSet& conf)
    : 
      name_(conf.getParameter<std::string>("name")),
      jetsTag_(conf.getParameter<edm::InputTag>("jets")),
      jetsToken_(consumes<edm::View<T>>(jetsTag_)),
      EECTag_(conf.getParameter<edm::InputTag>("EECs")),
      EECToken_(consumes<K>(EECTag_)),
      nDR_(conf.getParameter<unsigned>("nDR")),
      doParts_(conf.getParameter<bool>("doParts")){
  produces<nanoaod::FlatTable>(name_ + "COV");
  produces<nanoaod::FlatTable>(name_ + "BK");
  produces<nanoaod::FlatTable>(name_);
  if(doParts_){
    produces<nanoaod::FlatTable>(name_ + "PARTS");
  }
}

template <typename T, typename K>
void EECTableProducerT<T, K>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<edm::InputTag>("jets");
  desc.add<edm::InputTag>("EECs");
  desc.add<unsigned>("nDR");
  desc.add<bool>("doParts");
  descriptions.addWithDefaultLabel(desc);
}

template <typename T, typename K>
void EECTableProducerT<T, K>::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<T>> jets;
  evt.getByToken(jetsToken_, jets);

  edm::Handle<K> EECs;
  evt.getByToken(EECToken_, EECs);

  //EEC table
  auto flatDRs = std::make_unique<std::vector<std::vector<float>>>();
  flatDRs->resize(nDR_);
  auto flatWTs = std::make_unique<std::vector<float>>();

  //table for covariance computation
  auto flatCOV = std::make_unique<std::vector<float>>();

  //book-keeping table
  auto jetIdx = std::make_unique<std::vector<int>>();
  auto nDR = std::make_unique<std::vector<int>>();
  auto nCOV = std::make_unique<std::vector<int>>();
  auto nPart = std::make_unique<std::vector<int>>();

  //particles table
  auto partPt = std::make_unique<std::vector<float>>();
  auto partEta = std::make_unique<std::vector<float>>();
  auto partPhi = std::make_unique<std::vector<float>>();
  auto partPdgId = std::make_unique<std::vector<int>>();
  auto partMatched = std::make_unique<std::vector<int>>();

  int iJet=0;
  for (size_t iEEC = 0; iEEC < EECs->size(); ++iEEC){
    auto EEC = EECs->at(iEEC);
    while(iJet < EEC.iJet){ //for all skipped jets
      for(size_t i=0; i<nDR_; ++i){
        flatDRs->at(i).emplace_back(0);
      }
      flatWTs->emplace_back(0);
      flatCOV->emplace_back(0);

      jetIdx->emplace_back(iJet++);
      nDR->emplace_back(1);
      nCOV->emplace_back(1);

      if(doParts_){
        partPt->emplace_back(0);
        partEta->emplace_back(0);
        partPhi->emplace_back(0);
        partPdgId->emplace_back(0);
        partMatched->emplace_back(0);
        nPart->emplace_back(1);
      }
    } //end for all skipped jets

    jetIdx->emplace_back(EEC.iJet);

    if(doParts_){
    //  for(size_t i=0; i<EEC.partPt->size(); ++i){
    //    partPt->emplace_back(EEC.partPt->at(i));
    //    partEta->emplace_back(EEC.partEta->at(i));
    //    partPhi->emplace_back(EEC.partPhi->at(i));
    //    partPdgId->emplace_back(EEC.partPdgId->at(i));
    //    partMatched->emplace_back(EEC.partMatched->at(i));
    //  }
    //  nPart->emplace_back(EEC.partPt->size());
    }

    for(size_t i=0; i<EEC.wtvec->size(); ++i){//for each dR in the EEC
      flatWTs->emplace_back(EEC.wtvec->at(i));
      if constexpr (!std::is_same<K, ProjectedEECCollection>::value){
        for(size_t j=0; j<nDR_; ++j) { //for each dR axis
          flatDRs->at(j).emplace_back(EEC.dRvec->at(j)[i]);
        } //end for each dR axis
      } else{
        flatDRs->at(0).emplace_back(EEC.dRvec->at(i));
      }
    }// end for each dR in the EEC
    nDR->emplace_back(EEC.wtvec->size());

    //delete-1 jacknife variance
    //equivalent to asking for each particle "what would happen if it went away?"
    //TODO: double-check iteration range
    size_t NPart = EEC.tuplewts->NPart();
    size_t NDR = EEC.dRvec->size();
    std::vector<int> ord(EEC.order, 0);
    std::vector<int> cpy(EEC.order, 0);
    size_t maxIter=choose(NPart + EEC.order -1, EEC.order);

    std::vector<std::vector<double>> dN;
    dN.resize(NDR);
    for(size_t iDR=0; iDR<NDR; ++iDR){
      dN[iDR].resize(NPart, 0.0);
    }
    std::vector<double> dD(NPart, 0.0);

    for(size_t iter=0; iter<maxIter; ++iter){
      int iDR = EEC.tupleiDR->at(ord);
      double wt = EEC.tuplewts->at(ord);

      cpy = ord;

      auto last = std::unique(cpy.begin(), cpy.end());
      cpy.erase(last, cpy.end());
      for(int iOrd : cpy){
        dN[iDR][iOrd] += wt;
        dD[iOrd] += wt;
      }

      iterate_wdiag<int>(EEC.order, ord, NPart);
    }

    size_t N=0;
    for(size_t iPart=0; iPart<EEC.coefs->at(0).size(); ++iPart){
      for(size_t iDR=0; iDR<EEC.coefs->at(0).at(iPart).size(); ++iDR){
        double dEEC = (dN[iDR][iPart] - EEC.wtvec->at(iDR)*dD[iPart])/(1-dD[iPart]);
        flatCOV->emplace_back(dEEC);
        ++N;
      }
    }
    nCOV->emplace_back(N);

    ++iJet; //next jet
  }
  auto table = std::make_unique<nanoaod::FlatTable>(flatWTs->size(), name_, false);
  table->addColumn<float>("wts", *flatWTs, "Weight", nanoaod::FlatTable::FloatColumn);
  for(unsigned i=0; i<nDR_; ++i){
    table->addColumn<float>(vformat("dR%d", i+1), flatDRs->at(i), vformat("%dth-largest delta R", i+1), nanoaod::FlatTable::FloatColumn);
  }
  evt.put(std::move(table), name_);

  auto tableCOV = std::make_unique<nanoaod::FlatTable>(flatCOV->size(), name_+"COV", false);
  tableCOV->addColumn<float>("COV", *flatCOV, "Elements of particle, dR matrix needed for covariance computation", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(tableCOV), name_+"COV");

  auto tableBK = std::make_unique<nanoaod::FlatTable>(jetIdx->size(), name_+"BK", false);
  tableBK->addColumn<int>("jetIdx", *jetIdx, "Jet index", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nDR", *nDR, "# of dR vals", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nCOV", *nCOV, "#of cov vals", nanoaod::FlatTable::IntColumn);
  if(doParts_){
    tableBK->addColumn<int>("nPart", *nPart, "#of particles", nanoaod::FlatTable::IntColumn);
  }
  evt.put(std::move(tableBK), name_+"BK");

  if(doParts_){
    auto tablePARTS = std::make_unique<nanoaod::FlatTable>(partPt->size(), name_+"PARTS", false);
    tablePARTS->addColumn<float>("partPt", *partPt, "particle pt", nanoaod::FlatTable::FloatColumn);
    tablePARTS->addColumn<float>("partEta", *partEta, "particle eta", nanoaod::FlatTable::FloatColumn);
    tablePARTS->addColumn<float>("partPhi", *partPhi, "particle phi", nanoaod::FlatTable::FloatColumn);
    tablePARTS->addColumn<int>("partPdgId", *partPdgId, "particle id", nanoaod::FlatTable::IntColumn);
    tablePARTS->addColumn<int>("partMatched", *partMatched, "whether part got matched", nanoaod::FlatTable::IntColumn);
    evt.put(std::move(tablePARTS), name_+"PARTS");
  }

}  // end produce()

typedef EECTableProducerT<reco::PFJet, ProjectedEECCollection> ProjectedEECTableProducer;
typedef EECTableProducerT<reco::GenJet, ProjectedEECCollection> GenProjectedEECTableProducer;
typedef EECTableProducerT<pat::Jet, ProjectedEECCollection> PatProjectedEECTableProducer;

//typedef EECTableProducerT<reco::PFJet, ResolvedEECCollection> ResolvedEECTableProducer;
//typedef EECTableProducerT<reco::GenJet, ResolvedEECCollection> GenResolvedEECTableProducer;
//typedef EECTableProducerT<pat::Jet, ResolvedEECCollection> PatResolvedEECTableProducer;

DEFINE_FWK_MODULE(ProjectedEECTableProducer);
DEFINE_FWK_MODULE(GenProjectedEECTableProducer);
DEFINE_FWK_MODULE(PatProjectedEECTableProducer);

//DEFINE_FWK_MODULE(ResolvedEECTableProducer);
//DEFINE_FWK_MODULE(GenResolvedEECTableProducer);
//DEFINE_FWK_MODULE(PatResolvedEECTableProducer);
