// -*- C++ -*-
//
// Package:    SRothman/EECs
// Class:      TransferAnalyzer
//
/**\class TransferAnalyzer TransferAnalyzer.cc SRothman/EECs/plugins/TransferAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Simon Rothman
//         Created:  Tue, 18 Oct 2022 04:19:07 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "SRothman/DataFormats/interface/EEC.h"
#include "TH2.h"
#include "TH1.h"

//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.



class TransferAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit TransferAnalyzer(const edm::ParameterSet&);
      ~TransferAnalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::EDGetTokenT<EECTransferCollection> token_;  
      edm::EDGetTokenT<ProjectedEECCollection> genEECToken_, recoEECToken_;  
      edm::EDGetTokenT<edm::View<pat::Jet>> recoJetToken_;
      edm::EDGetTokenT<edm::View<reco::GenJet>> genJetToken_;

      double minpt_;

      TH2D *hist;
      TH1D *gen, *reco;
      TH1D *matchedgen, *matchedreco;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
TransferAnalyzer::TransferAnalyzer(const edm::ParameterSet& iConfig)
 : token_(consumes<EECTransferCollection>(iConfig.getParameter<edm::InputTag>("src"))),
   genEECToken_(consumes<ProjectedEECCollection>(iConfig.getParameter<edm::InputTag>("genEEC"))),
   recoEECToken_(consumes<ProjectedEECCollection>(iConfig.getParameter<edm::InputTag>("recoEEC"))),
   recoJetToken_(consumes<edm::View<pat::Jet>>(iConfig.getParameter<edm::InputTag>("recoJets"))),
   genJetToken_(consumes<edm::View<reco::GenJet>>(iConfig.getParameter<edm::InputTag>("genJets"))),
   minpt_(iConfig.getParameter<double>("minpt"))
{
  std::cout << "initializing analyzer" << std::endl;

  hist = new TH2D("histo", "transfer;dRgen;dRreco", 20, -3, 0, 20, -3, 0);
  gen = new TH1D("genEEC", "genEEC;dR", 20, -3, 0);
  reco = new TH1D("recoEEC", "recoEEC;dR", 20, -3, 0);
  matchedgen = new TH1D("genEEC", "genEEC;dR", 20, -3, 0);
  matchedreco = new TH1D("recoEEC", "recoEEC;dR", 20, -3, 0);
}


TransferAnalyzer::~TransferAnalyzer()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called for each event  ------------
void TransferAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  std::cout << "doing an event in the analyzer" << std::endl;
  auto transfers = iEvent.get(token_);
  auto recoJets = iEvent.get(recoJetToken_);
  auto genJets = iEvent.get(genJetToken_);

  for(const EECTransfer& transfer : transfers) {
    if(recoJets.at(transfer.iJetReco).pt() < minpt_){
      continue;
    }
    for(size_t iGen=0; iGen < transfer.dRgen->size(); ++iGen){
      for(size_t iReco=0; iReco < transfer.dRreco->size(); ++iReco){
        hist->Fill(log10(transfer.dRgen->at(iGen)), 
                   log10(transfer.dRreco->at(iReco)), 
                   (*transfer.matrix)(iGen,iReco));
      }
    }
    for(size_t iGen=0; iGen < transfer.dRgen->size(); ++iGen){
      matchedgen->Fill(log10(transfer.dRgen->at(iGen)), transfer.wtgen->at(iGen));
    }
    for(size_t iReco=0; iReco < transfer.dRreco->size(); ++iReco){
      matchedreco->Fill(log10(transfer.dRreco->at(iReco)), transfer.wtreco->at(iReco));
    }
  }
  
  auto genEEC = iEvent.get(genEECToken_);
  for(const auto& EEC : genEEC){
    if (genJets.at(EEC.iJet).pt() < minpt_){
      continue;
    }
    for(size_t iDR=0; iDR<EEC.dRvec->size(); ++iDR){
      gen->Fill(log10(EEC.dRvec->at(iDR)), EEC.wtvec->at(iDR));
    }
  }
  
  auto recoEEC = iEvent.get(recoEECToken_);
  for(const auto& EEC : recoEEC){
    if(recoJets.at(EEC.iJet).pt() < minpt_){
      continue;
    }
    for(size_t iDR=0; iDR<EEC.dRvec->size(); ++iDR){
      reco->Fill(log10(EEC.dRvec->at(iDR)), EEC.wtvec->at(iDR));
    }
  }

  std::vector<double> matmul;
  matmul.resize(20, 0.);
  std::vector<double> matmul2;
  matmul2.resize(20, 0.);

  std::cout << std::endl << "TRANSFER" << std::endl;
  for(int i=0; i<20; ++i){
    for(int j=0; j<20; ++j){
      if(reco->GetBinContent(j) > 0.){
        printf("%0.3f  ", hist->GetBinContent(i,j)/matchedreco->GetBinContent(j));
        if(matchedreco->GetBinContent(j) > 1e-7){
          matmul[i] += hist->GetBinContent(i,j)
                      *reco->GetBinContent(j)
                      /matchedreco->GetBinContent(j);
        }
        matmul2[i] += hist->GetBinContent(i,j);
      } else {
        printf("%0.3f  ", 0.);
      }
    }
    printf("\n");
  }
  std::cout << std::endl;

  std::cout << "GEN" << std::endl;
  for(int j=0; j<20; ++j){
    printf("%0.3f  ", gen->GetBinContent(j));
  }
  printf("\n");

  std::cout << "RECO" << std::endl;
  for(int j=0; j<20; ++j){
    printf("%0.3f  ", reco->GetBinContent(j));
  }
  printf("\n");

  std::cout << "MATMUL" << std::endl;
  for(int j=0; j<20; ++j){
    printf("%0.3f  ", matmul.at(j));
  }
  printf("\n");

  std::cout << "MATCHED GEN" << std::endl;
  for(int j=0; j<20; ++j){
    printf("%0.3f  ", matchedgen->GetBinContent(j));
  }
  printf("\n");

  std::cout << "MATMUL2" << std::endl;
  for(int j=0; j<20; ++j){
    printf("%0.3f  ", matmul2.at(j));
  }
  printf("\n");



  std::cout << std::endl;
}


// ------------ method called once each job just before starting event loop  ------------
void TransferAnalyzer::beginJob()
{
  std::cout << "beginning job" << std::endl;
}

// ------------ method called once each job just after ending the event loop  ------------
void TransferAnalyzer::endJob()
{
  std::cout << "ending job" << std::endl;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void TransferAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("src");
  desc.add<edm::InputTag>("genEEC");
  desc.add<edm::InputTag>("recoEEC");
  desc.add<edm::InputTag>("recoJets");
  desc.add<double>("minpt");
  desc.add<edm::InputTag>("genJets");
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
  std::cout << "filled descriptions" << std::endl;
}

//define this as a plug-in
DEFINE_FWK_MODULE(TransferAnalyzer);
