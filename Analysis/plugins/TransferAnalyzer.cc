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

#define EPSILON 1e-15

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

      unsigned nBins_;

      TH2D *hist;
      TH1D *gen, *reco;
      TH1D *matchedgen, *matchedreco, *matchedreco2;
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
   minpt_(iConfig.getParameter<double>("minpt")),
   nBins_(iConfig.getParameter<unsigned>("nBins"))
{
  hist = new TH2D("histo", "transfer;dRgen;dRreco", nBins_, -3, 0, nBins_, -3, 0);
  gen = new TH1D("genEEC", "genEEC;dR", nBins_, -3, 0);
  reco = new TH1D("recoEEC", "recoEEC;dR", nBins_, -3, 0);
  matchedgen = new TH1D("genEEC", "genEEC;dR", nBins_, -3, 0);
  matchedreco = new TH1D("recoEEC", "recoEEC;dR", nBins_, -3, 0);
  matchedreco2 = new TH1D("recoEEC", "recoEEC;dR", nBins_, -3, 0);
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
  auto transfers = iEvent.get(token_);
  auto recoJets = iEvent.get(recoJetToken_);
  auto genJets = iEvent.get(genJetToken_);

  for(const EECTransfer& transfer : transfers) {
    if(recoJets.at(transfer.iJetReco).pt() < minpt_){
      continue;
    }
    for(size_t iGen=0; iGen < transfer.dRgen->size(); ++iGen){
      for(size_t iReco=0; iReco < transfer.dRreco->size(); ++iReco){
        hist->Fill(
            log10(transfer.dRgen->at(iGen)),
            log10(transfer.dRreco->at(iReco)), 
            (*transfer.matrix)(iReco,iGen));
        matchedreco2->Fill(
            log10(transfer.dRreco->at(iReco)), 
            (*transfer.matrix)(iReco, iGen));
      }
    }

    for(size_t iGen=0; iGen < transfer.dRgen->size(); ++iGen){
      if(transfer.wtgen->at(iGen)>0){
        //printf("matchedgen (%0.5lf) = %0.5lf\n", transfer.dRgen->at(iGen), transfer.wtgen->at(iGen));
        //fflush(stdout);
      }
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
  matmul.resize(nBins_+2, 0.);
  std::vector<double> matmul2;
  matmul2.resize(nBins_+2, 0.);

  std::cout << std::endl << "TRANSFER" << std::endl;
  for(unsigned j=0; j<nBins_+2; ++j){
    for(unsigned i=0; i<nBins_+2; ++i){
      matmul2[j] += hist->GetBinContent(i,j);
      if(matchedgen->GetBinContent(i) > 0.){
        printf("%0.5g, ", hist->GetBinContent(i,j));
        if(matchedgen->GetBinContent(j) > EPSILON){
          matmul[i] += hist->GetBinContent(i,j)
                      *gen->GetBinContent(i)
                      /matchedgen->GetBinContent(i);
        }
      } else {
        printf("%0.5g, ", 0.);
      }
    }
    printf("\n");
  }
  std::cout << std::endl;

  std::cout << "GEN" << std::endl;
  for(unsigned j=0; j<nBins_+2; ++j){
    printf("%0.5g, ", gen->GetBinContent(j));
  }
  printf("\n");

  std::cout << "RECO" << std::endl;
  for(unsigned j=0; j<nBins_+2; ++j){
    printf("%0.5g, ", reco->GetBinContent(j));
  }
  printf("\n");

  std::cout << "MATMUL" << std::endl;
  for(unsigned j=0; j<nBins_+2; ++j){
    printf("%0.5g, ", matmul.at(j));
  }
  printf("\n");

  std::cout << "MATCHED GEN" << std::endl;
  for(unsigned j=0; j<nBins_+2; ++j){
    printf("%0.5g, ", matchedgen->GetBinContent(j));
  }
  printf("\n");

  std::cout << "MATMUL2" << std::endl;
  for(unsigned j=0; j<nBins_+2; ++j){
    printf("%0.5g, ", matmul2.at(j));
  }
  printf("\n");

  std::cout << "MATCHED RECO" << std::endl;
  for(unsigned j=0; j<nBins_+2; ++j){
    printf("%0.5g, ", matchedreco->GetBinContent(j));
  }
  printf("\n");

  std::cout << "MATCHED RECO 2" << std::endl;
  for(unsigned j=0; j<nBins_+2; ++j){
    printf("%0.5g, ", matchedreco2->GetBinContent(j));
  }
  printf("\n");

  std::cout << std::endl;
}


// ------------ method called once each job just before starting event loop  ------------
void TransferAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void TransferAnalyzer::endJob()
{
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
  desc.add<unsigned>("nBins");
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TransferAnalyzer);
