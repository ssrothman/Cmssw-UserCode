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

#include "SRothman/EECs/plugins/EECutil.h"

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
  edm::Handle<edm::View<jet>> reco;
  evt.getByToken(recoToken_, reco);

  auto result = std::make_unique<std::vector<EECresult>>();

  for(unsigned iReco=0; iReco<reco->size(); ++iReco){
      ProjectedEECCalculator projcalc(reco->at(iReco), maxOrder_);

      std::vector<NonIRCEECCalculator<false>> nonirccalcs;
      nonirccalcs.emplace_back(reco->at(iReco), 2u, 
                               getCustomComps(2, 1));
      nonirccalcs.emplace_back(reco->at(iReco), 2u, 
                               getCustomComps(2, 2));
      nonirccalcs.emplace_back(reco->at(iReco), 2u, 
                               getCustomComps(3, 1));
      std::vector<int> nonircorders = {-21, -22, -31};

      ResolvedEECCalculator rescalc(reco->at(iReco), 4u);

      EECresult next;
      next.maxOrder = maxOrder_;
      next.iJet = iReco;

      projcalc.run();
      addProjectedDRs(next, projcalc);

      for(unsigned order=2; order<=maxOrder_; ++order){
          addProjectedWTs(next, projcalc, order);
      }
      
      for(unsigned i=0; i<nonirccalcs.size(); ++i){
          auto& calc = nonirccalcs.at(i);
          calc.run();
          addProjectedWTs(next, calc, 2u);
      }

      arma::mat covp(next.wts.size(), reco->at(iReco).nPart, arma::fill::none);
      for(unsigned order=2; order<=maxOrder_; ++order){
          addCovP(covp, projcalc, order, next.offsets[order-2]);
      }

      unsigned ioff = maxOrder_+1;
      for(const auto& calc : nonirccalcs){
          addCovP(covp, calc, 2, next.offsets[ioff-2]);
          ++ioff;
      }
      next.cov = covp * arma::trans(covp);
      
      rescalc.run();
      addResolved3(next, rescalc);
      addResolved4(next, rescalc);
      
     
      arma::mat covp3 = rescalc.getCov(3);
      arma::mat covp4 = rescalc.getCov(4);

      next.covRes3Res3 = covp3 * arma::trans(covp3);
      next.covRes3Proj = covp3 * arma::trans(covp);

      next.covRes4Res4 = covp4 * arma::trans(covp4);
      next.covRes4Res3 = covp4 * arma::trans(covp3);
      next.covRes4Proj = covp4 * arma::trans(covp);

      result->push_back(std::move(next));

  }


  evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(RecoEECProducer);
