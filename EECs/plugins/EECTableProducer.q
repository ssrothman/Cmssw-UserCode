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

class EECProducer : public edm::stream::EDProducer<> {
public:
    explicit EECProducer(const edm::ParameterSet&);
    ~EECProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    int verbose_;

    unsigned maxOrder_;

    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;
    edm::InputTag genTag_;
    edm::EDGetTokenT<edm::View<jet>> genToken_;
    edm::InputTag matchTag_;
    edm::EDGetTokenT<edm::View<jetmatch>> matchToken_;
};

EECProducer::EECProducer(const edm::ParameterSet& conf)
        : verbose_(conf.getParameter<int>("verbose")),
          maxOrder_(conf.getParameter<unsigned>("maxOrder")),
          recoTag_(conf.getParameter<edm::InputTag>("reco")),
          recoToken_(consumes<edm::View<jet>>(recoTag_)),
          genTag_(conf.getParameter<edm::InputTag>("gen")),
          genToken_(consumes<edm::View<jet>>(genTag_)),
          matchTag_(conf.getParameter<edm::InputTag>("match")),
          matchToken_(consumes<edm::View<jetmatch>>(matchTag_)){
    produces<std::vector<EECresult>>();
}

void EECProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("reco");
  desc.add<edm::InputTag>("gen");
  desc.add<edm::InputTag>("match");
  desc.add<int>("verbose");
  desc.add<unsigned>("maxOrder");
  descriptions.addWithDefaultLabel(desc);
}

void EECProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<jet>> reco;
  evt.getByToken(recoToken_, reco);

  edm::Handle<edm::View<jet>> gen;
  evt.getByToken(genToken_, gen);

  edm::Handle<edm::View<jetmatch>> matches;
  evt.getByToken(matchToken_, matches);

  auto result = std::make_unique<std::vector<EECresult>>();

  for(unsigned iReco=0; iReco<reco->size(); ++iReco){
      ProjectedEECCalculator projcalc(reco->at(iReco), maxOrder_);
      projcalc.run();


      EECresult next;
      next.maxOrder = maxOrder_;
      next.iJet = iReco;

      for(unsigned order=2; order<=maxOrder_; ++order){
        next.offsets.push_back(next.wts.size());
        const std::vector<double>& wts = projcalc.getwts(order);
        next.wts.insert(next.wts.end(), wts.begin(), wts.end());
        const std::vector<double>& dRs = projcalc.getdRs();
        next.dRs.insert(next.dRs.end(), dRs.begin(), dRs.end());
      }

      arma::mat covp(next.wts.size(), reco->at(iReco).nPart, arma::fill::none);
      for(unsigned order=2; order<=maxOrder_; ++order){
          const arma::mat& cov = projcalc.getCov(order);
          for(unsigned i=0; i<cov.n_rows; ++i){
              for(unsigned j=0; j<cov.n_cols; ++j){
                  covp(i + next.offsets[order-2], j) = cov(i, j);
              }
          }
      }
      next.cov = covp * arma::trans(covp);

      result->push_back(std::move(next));
  }


  evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(EECProducer);
