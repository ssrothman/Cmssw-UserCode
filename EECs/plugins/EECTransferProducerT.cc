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
      nDR_(conf.getParameter<unsigned>("nDR")){
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
  desc.add<unsigned>("nDR");
  descriptions.addWithDefaultLabel(desc);
}

template <typename T, typename K>
void EECTransferProducerT<T, K>::produce(edm::Event& evt, const edm::EventSetup& setup) {
  std::cout << "producing" << std::endl;
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
    std::cout << "iFlow " << iFlow << std::endl;

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
      continue;
    }
    auto recoEEC = EECs->at(iEEC);

    std::cout << "did the setup "<< std::endl;

    size_t size = std::max(recoEEC.dRvec->size(), flow.ER->size());
    arma::vec EECvec(size, arma::fill::zeros), Evec(size, arma::fill::zeros);
    for(size_t iDR=0; iDR<recoEEC.dRvec->size(); ++iDR){
      EECvec[iDR] = recoEEC.wtvec->at(iDR);
    }
    for(size_t iPart=0; iPart<flow.ER->size(); ++iPart){
      Evec[iPart] = flow.ER->at(iPart);
    }

    arma::mat A = (Evec * Evec.t())/arma::as_scalar(Evec.t() * EECvec);

    std::cout << "made A matrix " << std::endl;

    //NB only implemented for 2nd-order correlators at the moment
    auto transfer = std::shared_ptr<std::vector<std::vector<double>>>();
    transfer->resize(genEEC.dRvec->size());
    for(size_t iGenDR=0; iGenDR<genEEC.dRvec->size(); ++iGenDR){//for gen DR
      std::cout <<"outermost loop" << std::endl;
      transfer->at(iGenDR).resize(recoEEC.dRvec->size());
      for(size_t iRecoDR=0; iRecoDR<recoEEC.dRvec->size(); ++iRecoDR){//for reco DR
        std::cout <<"second outermost loop" << std::endl;
        transfer->at(iGenDR)[iRecoDR] = 0;
        for(size_t iPGen=0; iPGen<flow.Ngen; ++iPGen){//for gen particle
          std::cout <<"third outermost loop" << std::endl;
          for(size_t iPReco=0; iPReco<flow.Nreco; ++iPReco){//for reco particle
            std::cout <<"fourth outermost loop" << std::endl;
            if(recoEEC.coefs->at(0)[iPReco][iRecoDR]>0){
              std::cout << "(iGenDR, iRecoDR, iPGen, iPReco) ";
              printf("(%lu, %lu, %lu, %lu)", iGenDR, iRecoDR, iPGen, iPReco);
              std::cout << std::endl;
              std::cout << "W " << genEEC.coefs->at(0)[iPGen][iGenDR] << std::endl;
              std::cout << "F " << flow.at(iPGen, iPReco) << std::endl;
              std::cout << "A " << A(iPReco, iRecoDR) << std::endl;
              transfer->at(iGenDR)[iRecoDR] += genEEC.coefs->at(0)[iPGen][iGenDR]
                                         * flow.at(iPGen, iPReco) 
                                         * A(iPReco, iRecoDR);
            }
          }
        }
      }
    }
    std::cout << "make transfer" << std::endl;
    result->emplace_back(genEEC.dRvec, recoEEC.dRvec, transfer);
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
