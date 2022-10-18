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

#include "SRothman/DataFormats/interface/EEC.h"
#include "TH2.h"

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
      edm::EDGetTokenT<EECTransferCollection> token_;  //used to select what tracks to read from configuration file

      TH2D *hist;
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
 : token_(consumes<EECTransferCollection>(iConfig.getParameter<edm::InputTag>("src")))
{
  std::cout << "initializing analyzer" << std::endl;

  hist = new TH2D("histo", "transfer;dRgen;dRreco", 20, 0, 1, 20, 0, 1);

   //now do what ever initialization is needed
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
  size_t i=0;
  for(const EECTransfer& transfer : transfers) {
    for(size_t iGen=0; iGen < transfer.dRgen->size(); ++iGen){
      for(size_t iReco=0; iReco < transfer.dRreco->size(); ++iReco){
        hist->Fill(transfer.dRgen->at(iGen), 
                   transfer.dRreco->at(iReco), 
                   transfer.matrix->at(iGen).at(iReco));
      }
    }
  }
  std::cout << std::endl;
  for(int i=0; i<10; ++i){
    for(int j=0; j<10; ++j){
      printf("%0.3f\t", hist->GetBinContent(i,j));
    }
    printf("\n");
  }
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
