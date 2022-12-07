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

#include "SRothman/EECs/src/iterating.h"

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

  auto result = std::make_unique<EECTransferCollection>();

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
    arma::mat W_R_PP(NDR_R, NPart_R, arma::fill::zeros); 
    for(size_t iPart=0; iPart<NPart_R; ++iPart){
      for(size_t iDR=0; iDR<NDR_R; ++iDR){
        if(E_R[iPart] > EPSILON){
          W_R_PP(iDR, iPart) = recoEEC.coefs->at(0)[iPart][iDR];
          W_R(iDR, iPart) = W_R_PP(iDR, iPart) / E_R[iPart];
        } else {
          W_R_PP(iDR, iPart) = 0;
          W_R(iDR, iPart) = 0;
        }
      }
    }

    arma::mat W_G(NDR_G, NPart_G, arma::fill::zeros); 
    arma::mat W_G_PP(NDR_G, NPart_G, arma::fill::zeros); 
    for(size_t iPart=0; iPart<NPart_G; ++iPart){
      for(size_t iDR=0; iDR<NDR_G; ++iDR){
        if(E_G[iPart] > EPSILON){
          W_G_PP(iDR, iPart) = genEEC.coefs->at(0)[iPart][iDR];// / E_G[iPart];
          W_G(iDR, iPart) = W_G_PP(iDR, iPart) / E_G[iPart];
        } else {
          W_G_PP(iDR, iPart) = 0;
          W_G(iDR, iPart) = 0;
        }
      }
    }
    arma::vec WGsum = arma::sum(W_G_PP, 1);

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

    //actually fill transfer matrix
    auto transfer = std::make_shared<arma::mat>();
    std::shared_ptr<arma::mat> Tmat;
    if(mode_ == "proj"){
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

      *Tmat = W_G * F * Aproj;

    } else if(mode_ == "MP"){
      //build inverse with Moore-Penrse
      arma::mat Winv = arma::pinv(W_R);
      arma::vec y = E_R - Winv*EEC_R;
      double dot2 = std::abs(arma::as_scalar(y.t() * E_R));
      arma::mat AMP;
      if(dot2 > EPSILON){
        AMP = Winv + arma::reshape((E_R * E_R.t())/dot2, NPart_R, NDR_R);
      } else{
        AMP = Winv;
      }

      *Tmat = W_G*F*AMP;
    } else if(mode_ == "AF" || mode_ == "FF"){
      //Invert with absolute flow
      arma::mat W_R_PP_G = W_R*arma::trans(F);
      for(size_t iPart=0; iPart<NPart_G; ++iPart){
        for(size_t iDR=0; iDR<NDR_R; ++iDR){
          W_R_PP_G(iDR, iPart) *= E_G[iPart];
        }
      }
      arma::vec WRGsum = arma::sum(W_R_PP_G, 1);

      arma::mat W_G_PP_G = arma::trans(W_G_PP);
      arma::rowvec WGsum = arma::sum(W_G_PP_G, 0);

      auto TAF = std::make_shared<arma::mat>(NDR_R, NDR_G, arma::fill::zeros);
      for(size_t iPart=0; iPart<NPart_G; ++iPart){
        arma::rowvec G = W_G_PP_G.row(iPart);
        arma::vec R = W_R_PP_G.col(iPart);

        arma::rowvec G2 = arma::square(G);
        double den = arma::dot(G, G);
        if (den>EPSILON){
          arma::mat next = R * G2 / den;

          arma::vec RG = next * arma::ones<arma::vec>(NDR_G);

          *TAF += R * G2 / den;
        }
      }

      //invert with fractional flow
      auto TFF = std::make_shared<arma::mat>(NDR_R, NDR_G, arma::fill::zeros);
      for(size_t iDR_G=0; iDR_G<NDR_G; ++iDR_G){
        for(size_t iDR_R=0; iDR_R<NDR_R; ++iDR_R){
          if(EEC_G(iDR_G) > EPSILON){
            (*TFF)(iDR_R, iDR_G) = (*TAF)(iDR_R, iDR_G)/EEC_G(iDR_G);
          } else {
            (*TFF)(iDR_R, iDR_G) = 0;
          }
        }
      }

      if (mode_ == "AF"){
        Tmat = TAF;
      } else {
        Tmat = TFF;
      }
    } else if (mode_ == "tuples") { 
      auto Ttuple = std::make_shared<arma::mat>(NDR_R, NDR_G, arma::fill::zeros);
      
      //recoEEC.order must equal genEEC.order
      std::vector<int> ord_R(recoEEC.order, 0);
      std::vector<int> ord_G(genEEC.order, 0);
      for(int i=0; i<recoEEC.order; ++i){
        ord_R[i] = 0;
      }

      //size_t maxIter_R = choose(NPart_R + recoEEC.order - 1, recoEEC.order);
      size_t maxIter_R = intPow(NPart_R, recoEEC.order);
      size_t maxIter_G = choose(NPart_G + genEEC.order - 1, genEEC.order);
      //size_t maxIter_G = intPow(NPart_G, genEEC.order);

      for(int i=0; i<genEEC.order; ++i){
        ord_R[i] = 0;
        ord_G[i] = 0;
      }

      for(size_t iter_R=0; iter_R<maxIter_R; ++iter_R){//for each reco configuration
        for(size_t iter_G=0; iter_G<maxIter_G; ++iter_G){//for each gen configuration
          int iDR_R = recoEEC.tupleiDR->at(ord_R);
          int iDR_G = genEEC.tupleiDR->at(ord_G);
          double wt = genEEC.tuplewts->at(ord_G);

          if(wt>EPSILON && recoEEC.tuplewts->at(ord_R) > EPSILON){

            for(int iOrd=0; iOrd<recoEEC.order; ++iOrd){
              wt*= F(ord_G[iOrd], ord_R[iOrd]);
            }

            if(wt>EPSILON){
              printOrd(ord_G);
              printf(" -> ");
              printOrd(ord_R);
              printf("\n\tE1G = %0.4f\tF = %0.4f\tF*E1G = %0.4f", 
                  E_G(ord_G[0]), F(ord_G[0], ord_R[0]), 
                  F(ord_G[0], ord_R[0]) * E_G(ord_G[0]));
              printf("\n\tE2G = %0.4f\tF = %0.4f\tF*E2G = %0.4f", 
                  E_G(ord_G[1]), F(ord_G[1], ord_R[1]), 
                  F(ord_G[1], ord_R[1]) * E_G(ord_G[1]));
              printf("\n\twt_G = %0.4f\tF*F*wt = %0.4f\n", genEEC.tuplewts->at(ord_G), wt);
              (*Ttuple)(iDR_R, iDR_G) += wt;
            }
          }

          iterate_wdiag<int>(genEEC.order, ord_G, NPart_G);
        }
        for(int i=0; i<genEEC.order; ++i){
          ord_G[i] = 0;
        }
        iterate_all<int>(recoEEC.order, ord_R, NPart_R);
      }

      for(int i=0; i<recoEEC.order; ++i){
        ord_R[i]=0;
      }

      Tmat = Ttuple;

      //std::cout << "Ttuple" << std::endl << *Ttuple << std::endl;
      std::cout << "EEC_R" << std::endl << EEC_R << std::endl;
      std::cout << "sum(Ttuple, 1)" << std::endl << arma::sum(*Ttuple, 1) << std::endl;

    }else{
      throw cms::Exception("EECTransferProducer") << "unsupported mode" << std::endl;
    }

    result->emplace_back(genEEC.iJet, recoEEC.iJet,
                          genEEC.dRvec, recoEEC.dRvec, 
                          genEEC.wtvec, recoEEC.wtvec, 
                          Tmat);
  }

  evt.put(std::move(result));
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
