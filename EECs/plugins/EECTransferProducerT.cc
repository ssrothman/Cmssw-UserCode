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
#include "SRothman/DataFormats/interface/EMDFlow.h"

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <cstdarg>

extern "C" {
#include <lapacke.h>
#include <cblas.h>
}

#include "SRothman/armadillo/include/armadillo"

#define MAX_CONSTITUENTS 128
#define EPSILON 1e-15

#define VERBOSE 

template <typename T, typename K>
class EECTransferProducerT : public edm::stream::EDProducer<> {
public:
  explicit EECTransferProducerT(const edm::ParameterSet&);
  ~EECTransferProducerT() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  edm::InputTag jetsTag_;
  edm::EDGetTokenT<edm::View<T>> jetsToken_;

  edm::InputTag genJetsTag_;
  edm::EDGetTokenT<edm::View<reco::GenJet>> genJetsToken_;

  edm::InputTag EECTag_;
  edm::EDGetTokenT<K> EECToken_;

  edm::InputTag genEECTag_;
  edm::EDGetTokenT<K> genEECToken_;

  edm::InputTag flowTag_;
  edm::EDGetTokenT<EMDFlowCollection> flowToken_;

  unsigned nDR_;

  std::string mode_;
};

template <typename T, typename K>
EECTransferProducerT<T, K>::EECTransferProducerT(const edm::ParameterSet& conf)
    : 
      jetsTag_(conf.getParameter<edm::InputTag>("jets")),
      jetsToken_(consumes<edm::View<T>>(jetsTag_)),
      genJetsTag_(conf.getParameter<edm::InputTag>("genJets")),
      genJetsToken_(consumes<edm::View<reco::GenJet>>(genJetsTag_)),
      EECTag_(conf.getParameter<edm::InputTag>("EECs")),
      EECToken_(consumes<K>(EECTag_)),
      genEECTag_(conf.getParameter<edm::InputTag>("genEECs")),
      genEECToken_(consumes<K>(genEECTag_)),
      flowTag_(conf.getParameter<edm::InputTag>("flows")),
      flowToken_(consumes<EMDFlowCollection>(flowTag_)),
      nDR_(conf.getParameter<unsigned>("nDR")),
      mode_(conf.getParameter<std::string>("mode")){
  produces<nanoaod::FlatTable>();
  produces<EECTransferCollection>();
}

template <typename T, typename K>
void EECTransferProducerT<T, K>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("jets");
  desc.add<edm::InputTag>("genJets");
  desc.add<edm::InputTag>("EECs");
  desc.add<edm::InputTag>("genEECs");
  desc.add<edm::InputTag>("flows");
  desc.add<std::string>("mode");
  desc.add<unsigned>("nDR");
  descriptions.addWithDefaultLabel(desc);
}

template <typename T, typename K>
void EECTransferProducerT<T, K>::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<T>> jets;
  evt.getByToken(jetsToken_, jets);

  edm::Handle<edm::View<reco::GenJet>> genJets;
  evt.getByToken(genJetsToken_, genJets);

  edm::Handle<K> EECs;
  evt.getByToken(EECToken_, EECs);

  edm::Handle<K> genEECs;
  evt.getByToken(genEECToken_, genEECs);

  edm::Handle<EMDFlowCollection> flows;
  evt.getByToken(flowToken_, flows);

  auto result2 = std::make_unique<EECTransferCollection>();

  size_t iEEC;
  for(size_t iFlow=0; iFlow < flows->size(); ++iFlow){

    auto flow = flows->at(iFlow);

    //find corresponding EECs
    bool found=false;
    for(iEEC=0; iEEC<genEECs->size(); ++iEEC){
      if(genEECs->at(iEEC).iJet == flow.iGen){
        found=true;
        break;
      }
    }
    if (!found){
      //throw cms::Exception("EECTransferProducer") << "Couldn't find genEEC\n";
      continue;
    }
    auto genEEC = genEECs->at(iEEC);

    found=false;
    for(iEEC=0; iEEC<EECs->size(); ++iEEC){
      if(EECs->at(iEEC).iJet == flow.iReco){
        found=true;
        break;
      }
    }
    if (!found){
      //throw cms::Exception("EECTransferProducer") << "Couldn't find recoEEC\n";
      continue;
    }
    auto recoEEC = EECs->at(iEEC);

    size_t NDR_R = recoEEC.dRvec->size();
    size_t NPart_R = flow.Nreco;
    size_t max_R = std::max(NDR_R, NPart_R);

    size_t NDR_G = genEEC.dRvec->size();
    size_t NPart_G = flow.Ngen;
    //compute moore-penrose pseudoinversion of recoEEC
    //currently only for second-order correlators

    arma::vec E_R(NPart_R, arma::fill::zeros);
    for(size_t iPart=0; iPart<NPart_R; ++iPart){
      E_R(iPart) = flow.ER->at(iPart);
    }

    arma::vec E_G(NPart_G, arma::fill::zeros);
    for(size_t iPart=0; iPart<NPart_G; ++iPart){
      E_G(iPart) = flow.EG->at(iPart);
    }

    //build Wreco
    arma::mat W_R(NDR_R, NPart_R, arma::fill::zeros); 
    for(size_t iPart=0; iPart<NPart_R; ++iPart){
      for(size_t iDR=0; iDR<NDR_R; ++iDR){
        if(E_R[iPart] > EPSILON){
          W_R(iDR, iPart) = recoEEC.coefs->at(0)[iPart][iDR];// / E_R[iPart];
        } else{
          W_R(iDR, iPart) = 0.;
        }
      }
    }

    arma::mat W_G(NDR_G, NPart_G, arma::fill::zeros); 
    for(size_t iPart=0; iPart<NPart_G; ++iPart){
      for(size_t iDR=0; iDR<NDR_G; ++iDR){
        if(E_G[iPart] > EPSILON){
          W_G(iDR, iPart) = genEEC.coefs->at(0)[iPart][iDR];// / E_G[iPart];
        } else{
          W_G(iDR, iPart) = 0.;
        }
      }
    }

    arma::mat F(NPart_G, NPart_R, arma::fill::zeros);
    for(size_t iG=0; iG<NPart_G; ++iG){
      for(size_t iR=0; iR<NPart_R; ++iR){
        F(iG, iR) = flow.at(iG, iR);
      }
    }

    arma::vec EEC_G(NDR_G, arma::fill::zeros);
    for(size_t iDR=0; iDR<NDR_G; ++iDR){
      EEC_G(iDR) = genEEC.wtvec->at(iDR);
    }

    arma::vec EEC_R(NDR_R, arma::fill::zeros);
    for(size_t iDR=0; iDR<NDR_R; ++iDR){
      EEC_R(iDR) = recoEEC.wtvec->at(iDR);
    }

    //build inverse via projection
    arma::vec EEC_R_pad = arma::reshape(EEC_R, max_R, 1);
    arma::vec E_R_pad = arma::reshape(E_R, max_R, 1);
    double dot = std::abs(arma::as_scalar(EEC_R_pad.t() * E_R_pad));
    arma::mat Aproj;
    if(dot > EPSILON){
      Aproj = E_R * E_R.t()/dot;
    } else{
      Aproj = arma::eye(max_R, max_R);
    }
    Aproj = arma::reshape(Aproj, NPart_R, NDR_R);

    auto Tproj = std::make_shared<arma::mat>();
    *Tproj = W_G * F * Aproj;

    //build inverse with Moore-Penrse
    arma::mat Winv = arma::pinv(W_R);
    arma::vec y = E_R - Winv*EEC_R;
    double dot2 = std::abs(arma::as_scalar(y.t() * E_R));
    arma::mat AMP;
    if(dot2 > EPSILON){
      AMP = Winv + arma::reshape((E_R * E_R.t())/dot, NPart_R, NDR_R);
    } else{
      AMP = Winv;
    }

    auto TMP = std::make_shared<arma::mat>();
    *TMP = W_G*F*AMP;

    //Invert with absolute flow
    arma::mat EEC_G_PP = W_G*F; //gen EEC per reco particle
    for(size_t iPart=0; iPart<NPart_R; ++iPart){
      for(size_t iDR=0; iDR<NDR_G; ++iDR){
        EEC_G_PP(iDR, iPart) = EEC_G_PP(iDR, iPart) * E_R(iPart);
      }
    }
    arma::mat EEC_R_PP(NPart_R, NDR_R, arma::fill::zeros); //reco EEC per reco particle
    for(size_t iPart=0; iPart<NPart_R; ++iPart){
      for(size_t iDR=0; iDR<NDR_R; ++iDR){
        EEC_R_PP(iPart, iDR) = W_R(iDR, iPart) * E_R(iPart);
      }
    }

    auto TAF = std::make_shared<arma::mat>(NDR_G, NDR_R, arma::fill::zeros);
    for(size_t iPart=0; iPart<NPart_R; ++iPart){
      arma::rowvec R = EEC_R_PP.row(iPart);
      arma::vec G = EEC_G_PP.col(iPart);

      double den = arma::dot(R, R);
      arma::rowvec R2 = arma::square(R);
      *TAF += G * R2 / den;
    }
    //fudge by some normalization factor I don't understand at the moment
    arma::vec pred = *TAF*arma::ones<arma::vec>(NDR_R);
    //
    //invert with fractional flow
    auto TFF = std::make_shared<arma::mat>(NDR_G, NDR_R, arma::fill::zeros);
    for(size_t iDR_G=0; iDR_G<NDR_G; ++iDR_G){
      for(size_t iDR_R=0; iDR_R<NDR_R; ++iDR_R){
        if(EEC_R(iDR_R) > EPSILON){
          (*TFF)(iDR_G, iDR_R) = (*TAF)(iDR_G, iDR_R)/EEC_R(iDR_R);
        } else {
          (*TFF)(iDR_G, iDR_R) = 0;
        }
      }
    }

    //actually fill transfer matrix
    auto transfer = std::make_shared<arma::mat>();
    std::shared_ptr<arma::mat> Tmat;
    if(mode_ == "proj"){
      Tmat = Tproj;
    } else if(mode_ == "MP"){
      Tmat = TMP;
    } else if(mode_ == "AF"){
      Tmat = TAF;
    } else if(mode_ == "FF"){
      Tmat = TFF;
    } else{
      throw cms::Exception("EECTransferProducer") << "unsupported mode" << std::endl;
    }
    result2->emplace_back(genEEC.iJet, recoEEC.iJet,
                          genEEC.dRvec, recoEEC.dRvec, 
                          genEEC.wtvec, recoEEC.wtvec, 
                          Tmat);
  
    printf("\nRECO\n");
    std::cout << EEC_R.t() << std::endl;
    printf("GEN\n");
    std::cout << EEC_G.t() << std::endl;
    printf("Tproj * RECO\n");
    std::cout << arma::trans(*Tproj * EEC_R) << std::endl;
    printf("TMP * RECO\n");
    std::cout << arma::trans(*TMP * EEC_R) << std::endl;
    std::cout << "TAF * 1s" << std::endl 
      << arma::trans(*TAF * arma::ones<arma::vec>(NDR_R)) << std::endl;
    std::cout << "TFF * EEC_R" << std::endl 
      << arma::trans(*TFF * EEC_R) << std::endl << std::endl;
    printf("\n");
  }

  auto flatDRs = std::make_unique<std::vector<float>>();
  auto flatWTs = std::make_unique<std::vector<float>>();
  auto jetIdx = std::make_unique<std::vector<int>>();

  auto table = std::make_unique<nanoaod::FlatTable>(flatWTs->size(), "transfer", false);
  table->addColumn<float>("wts", *flatWTs, "Weight", nanoaod::FlatTable::FloatColumn);
  table->addColumn<int>("jetIdx", *jetIdx, "jet index", nanoaod::FlatTable::IntColumn);
  table->addColumn<float>("dR", *flatDRs, "largest delta R", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(table));
  evt.put(std::move(result2));
}  // end produce()

typedef EECTransferProducerT<reco::PFJet, ProjectedEECCollection> ProjectedEECTransferProducer;
typedef EECTransferProducerT<reco::GenJet, ProjectedEECCollection> GenProjectedEECTransferProducer;
typedef EECTransferProducerT<pat::Jet, ProjectedEECCollection> PatProjectedEECTransferProducer;

//typedef EECTransferProducerT<reco::PFJet, ResolvedEECCollection> ResolvedEECTransferProducer;
//typedef EECTransferProducerT<reco::GenJet, ResolvedEECCollection> GenResolvedEECTransferProducer;
//typedef EECTransferProducerT<pat::Jet, ResolvedEECCollection> PatResolvedEECTransferProducer;

DEFINE_FWK_MODULE(ProjectedEECTransferProducer);
DEFINE_FWK_MODULE(GenProjectedEECTransferProducer);
DEFINE_FWK_MODULE(PatProjectedEECTransferProducer);

//DEFINE_FWK_MODULE(ResolvedEECTransferProducer);
//DEFINE_FWK_MODULE(GenResolvedEECTransferProducer);
//DEFINE_FWK_MODULE(PatResolvedEECTransferProducer);
