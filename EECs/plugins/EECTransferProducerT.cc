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
  std::string name_;

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
      name_(conf.getParameter<std::string>("name")),
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
  produces<nanoaod::FlatTable>(name_);
}

template <typename T, typename K>
void EECTransferProducerT<T, K>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
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

  std::vector<std::vector<std::vector<float>>> result;

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

    //compute moore-penrose pseudoinversion of recoEEC
    //currently only for second-order correlators
    arma::fmat Wreco(flow.Nreco, recoEEC.dRvec->size(), arma::fill::zeros); 
    for(size_t iPart=0; iPart<flow.Nreco; ++iPart){
      for(size_t iDR=0; iDR<recoEEC.dRvec->size(); ++iDR){
        Wreco(iPart, iDR) = recoEEC.coefs->at(0)[iPart][iDR];
      }
    }

    std::cout << "W matrix " << std::endl << Wreco << std::endl;
    arma::fmat Winv = arma::pinv(Wreco);
    std::cout << "Winv" << std::endl << Winv << std::endl;

    //NB only implemented for 2nd-order correlators at the moment
    std::vector<std::vector<float>> transfer;
    transfer.resize(genEEC.dRvec->size());
    printf("FLOW NGEN = %lu NRECO = %lu\n", flow.Ngen, flow.Nreco);
    for(size_t iGenDR=0; iGenDR<genEEC.dRvec->size(); ++iGenDR){//for gen DR
      transfer[iGenDR].resize(recoEEC.dRvec->size());
      for(size_t iRecoDR=0; iRecoDR<recoEEC.dRvec->size(); ++iRecoDR){//for reco DR
        transfer[iGenDR][iRecoDR] = 0;
        for(size_t iPGen=0; iPGen<flow.Ngen; ++iPGen){//for gen particle
          for(size_t iPReco=0; iPReco<flow.Nreco; ++iPReco){//for reco particle
            if(recoEEC.coefs->at(0)[iPReco][iRecoDR]>0){
              transfer[iGenDR][iRecoDR] += genEEC.coefs->at(0)[iPGen][iGenDR]
                                         * flow.at(iPGen, iPReco) 
                                         * Winv(iRecoDR, iPReco);
            }
          }
        }
      }
    }
    result.push_back(transfer);
    printf("TRANSFER MATRIX (reco x gen) = (%lu x %lu)\n",
        genEEC.dRvec->size(), recoEEC.dRvec->size());
    for(size_t iGenDR=0; iGenDR<genEEC.dRvec->size(); ++iGenDR){//for gen DR
      for(size_t iRecoDR=0; iRecoDR<recoEEC.dRvec->size(); ++iRecoDR){//for reco DR
        printf("%0.3f\t", transfer[iGenDR][iRecoDR]);
      }
      printf("\n");
    }

    std::vector<float> matmul;
    matmul.resize(genEEC.dRvec->size());

    printf("\n");
    printf("RECO WEIGHTS\n");
    for(size_t iRecoDR=0; iRecoDR<recoEEC.dRvec->size(); ++iRecoDR){//for reco DR
      printf("%0.3f\t", recoEEC.wtvec->at(iRecoDR));
    }
    printf("\n\n");

    printf("TRANSFER * RECO = \n");
    for(size_t iGenDR=0; iGenDR<genEEC.dRvec->size(); ++iGenDR){//for gen DR
      matmul[iGenDR] = 0;
      for(size_t iRecoDR=0; iRecoDR<recoEEC.dRvec->size(); ++iRecoDR){//for reco DR
        matmul[iGenDR] += transfer[iGenDR][iRecoDR] * recoEEC.wtvec->at(iRecoDR);
      }
      printf("%0.3f\t",matmul[iGenDR]);
    }
    printf("\n\n");

    printf("GEN WEIGHTS\n");
    for(size_t iGenDR=0; iGenDR<genEEC.dRvec->size(); ++iGenDR){//for gen DR
      printf("%0.3f\t", genEEC.wtvec->at(iGenDR));
    }
    printf("\n\n");
  }

  auto flatDRs = std::make_unique<std::vector<float>>();
  auto flatWTs = std::make_unique<std::vector<float>>();
  auto jetIdx = std::make_unique<std::vector<int>>();

  auto table = std::make_unique<nanoaod::FlatTable>(flatWTs->size(), name_, false);
  table->addColumn<float>("wts", *flatWTs, "Weight", nanoaod::FlatTable::FloatColumn);
  table->addColumn<int>("jetIdx", *jetIdx, "jet index", nanoaod::FlatTable::IntColumn);
  table->addColumn<float>("dR", *flatDRs, "largest delta R", nanoaod::FlatTable::FloatColumn);
  evt.put(std::move(table), name_);
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
