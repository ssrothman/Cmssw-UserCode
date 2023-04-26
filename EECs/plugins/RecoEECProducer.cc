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
#include "DataFormats/Math/interface/deltaR.h"

#include "SRothman/DataFormats/interface/jets.h"
#include "SRothman/DataFormats/interface/matching.h"
#include "SRothman/DataFormats/interface/EEC.h"

#include "SRothman/Matching/src/simon_util_cpp/deltaR.h"
#include "SRothman/Matching/src/simon_util_cpp/util.h"

#include "SRothman/EECs/src/eec_oo.h"

#include <iostream>
#include <memory>
#include <vector>

class RecoEECProducer : public edm::stream::EDProducer<> {
public:
    explicit RecoEECProducer(const edm::ParameterSet&);
    ~RecoEECProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    int verbose_;

    unsigned maxOrder_;

    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;
};

RecoEECProducer::RecoEECProducer(const edm::ParameterSet& conf)
        : verbose_(conf.getParameter<int>("verbose")),
          maxOrder_(conf.getParameter<unsigned>("maxOrder")),
          recoTag_(conf.getParameter<edm::InputTag>("reco")),
          recoToken_(consumes<edm::View<jet>>(recoTag_)){
    produces<std::vector<EECresult>>();
}

void RecoEECProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("reco");
  desc.add<int>("verbose");
  desc.add<unsigned>("maxOrder");
  descriptions.addWithDefaultLabel(desc);
}

void RecoEECProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
    printf("top of produce\n");
  edm::Handle<edm::View<jet>> reco;
  evt.getByToken(recoToken_, reco);
  printf("got from event\n");

  auto result = std::make_unique<std::vector<EECresult>>();

  for(unsigned iReco=0; iReco<reco->size(); ++iReco){
      printf("top of loop\n");
      ProjectedEECCalculator projcalc(reco->at(iReco), maxOrder_);
      projcalc.run();
      printf("ran projected\n");

      std::vector<NonIRCEECCalculator<false>> nonirccalcs;
      nonirccalcs.emplace_back(reco->at(iReco), 2u, 
                               getCustomComps(2, 1));
      nonirccalcs.emplace_back(reco->at(iReco), 2u, 
                               getCustomComps(2, 2));
      nonirccalcs.emplace_back(reco->at(iReco), 2u, 
                               getCustomComps(3, 1));
      std::vector<int> nonircorders = {-21, -22, -31};
      printf("made nonIRC\n");

      EECresult next;
      next.maxOrder = maxOrder_;
      next.iJet = iReco;

       const std::vector<double>& dRs = projcalc.getdRs();
       next.dRs.insert(next.dRs.end(), dRs.begin(), dRs.end());
       printf("got dRs\n");

      for(unsigned order=2; order<=maxOrder_; ++order){
        next.offsets.push_back(next.wts.size());
        const std::vector<double>& wts = projcalc.getwts(order);
        next.order.push_back(order);
        next.wts.insert(next.wts.end(), wts.begin(), wts.end());
        printf("got projected %u\n",order);
      }

      
      for(unsigned i=0; i<nonirccalcs.size(); ++i){
          auto& calc = nonirccalcs.at(i);
          printf("calculated nonIRC %d\n", nonircorders[i]);
          calc.run();
          next.offsets.push_back(next.wts.size());
          const std::vector<double>& wts = calc.getwts(2);
          next.wts.insert(next.wts.end(), wts.begin(), wts.end());
          next.order.push_back(nonircorders[i]);
          printf("got nonIRC %d\n", nonircorders[i]);
      }

      arma::mat covp(next.wts.size(), reco->at(iReco).nPart, arma::fill::none);
      for(unsigned order=2; order<=maxOrder_; ++order){
          const arma::mat& cov = projcalc.getCov(order);
          for(unsigned i=0; i<cov.n_rows; ++i){
              for(unsigned j=0; j<cov.n_cols; ++j){
                  covp(i + next.offsets[order-2], j) = cov(i, j);
              }
          }
          printf("got cov %u\n", order);
      }

      unsigned ioff = maxOrder_+1;
      for(const auto& calc : nonirccalcs){
          printf("ioff = %u\n", ioff);
          const arma::mat& cov = calc.getCov(2);
          for(unsigned i=0; i<cov.n_rows; ++i){
              for(unsigned j=0; j<cov.n_cols; ++j){
                  covp(i + next.offsets[ioff-2], j) = cov(i, j);
              }
          }
          printf("got cov '%u\n' ;)", ioff);
          ++ioff;
      }
      next.cov = covp * arma::trans(covp);
      printf("did cov multiply\n");

      result->push_back(std::move(next));
  }


  evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(RecoEECProducer);
