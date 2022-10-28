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

#include "SRothman/EECs/src/eec.h"
#include "SRothman/EECs/src/combinatorics.h"
#include "SRothman/EECs/src/vecND.h"

#include "SRothman/DataFormats/interface/EEC.h"

#include <iostream>
#include <memory>
#include <vector>

#define MAX_CONSTITUENTS 128

#define VERBOSE 

template <typename T, typename K>
class EECProducerT : public edm::stream::EDProducer<> {
public:
  explicit EECProducerT(const edm::ParameterSet&);
  ~EECProducerT() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  unsigned int order_;
  double minPartPt_;

  unsigned int verbose_;
  unsigned int p1_, p2_;

  edm::InputTag src_;
  edm::EDGetTokenT<edm::View<T>> srcToken_;

  double pt_[MAX_CONSTITUENTS], eta_[MAX_CONSTITUENTS], phi_[MAX_CONSTITUENTS];

  size_t fill(const std::vector<reco::Jet::Constituent>& constituents);

};

template <typename T, typename K>
size_t EECProducerT<T, K>::fill(const std::vector<reco::Jet::Constituent>& constituents){
  size_t nConstituents = std::min<size_t>(constituents.size(), MAX_CONSTITUENTS);

  double rawPt = 0;
  for (size_t i = 0; i < nConstituents; ++i) { //for each constituent
    auto part = constituents[i];
    rawPt += part->pt();
  } //end for each constituent
  size_t j=0;
  for (size_t i = 0; i < nConstituents; ++i) { //for each constituent
    auto part = constituents[i];
    if(part->pt() < minPartPt_){
      pt_[j] = 0.0; //ensure contribution is exactly zero
      eta_[j] = 999999999.99; //ensure zeroed-out particles are very far away from all the others so they don't participate in min DR computations
      phi_[j++] = 999999999.99; 
    } else{
      pt_[j] = (double)part->pt() / rawPt;
      eta_[j] = (double)part->eta();
      phi_[j++] = (double)part->phi();
    }
  } //end for each constituent

  return j;
}

template <typename T, typename K>
EECProducerT<T, K>::EECProducerT(const edm::ParameterSet& conf)
    : order_(conf.getParameter<unsigned int>("order")),
      minPartPt_(conf.getParameter<double>("minPartPt")),
      verbose_(conf.getParameter<unsigned int>("verbose")),
      p1_(conf.getParameter<unsigned int>("p1")),
      p2_(conf.getParameter<unsigned int>("p2")),
      src_(conf.getParameter<edm::InputTag>("jets")),
      srcToken_(consumes<edm::View<T>>(src_)){
  produces<K>();

  if( (p1_!=1 || p2_!=1) && order_!=2)
    throw cms::Exception("EECProducer") << "Only 2-point nonIRC EECs are supported at the moment" << std::endl;
}

template <typename T, typename K>
void EECProducerT<T, K>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<unsigned int>("order");
  desc.add<double>("minPartPt");
  desc.add<edm::InputTag>("jets");
  desc.add<unsigned int>("verbose");
  desc.add<unsigned int>("p1");
  desc.add<unsigned int>("p2");
  descriptions.addWithDefaultLabel(desc);
}

template <typename T, typename K>
void EECProducerT<T, K>::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<T>> jets;
  evt.getByToken(srcToken_, jets);

  unsigned nJets = jets->size();

  if(verbose_)
    std::cout << " Event has " << nJets << " jets" << std::endl;

  auto result = std::make_unique<K>();

  for (size_t iJet = 0; iJet < nJets; ++iJet) {

    T jet = jets->at(iJet);

    std::vector<reco::Jet::Constituent> constituents = jet.getJetConstituents();
    size_t nConstituents = fill(constituents);

    if (nConstituents<2){ //skip jet
      continue;
    } 

    if (verbose_){
      std::cout << "\tjet: (" << jet.pt() << ", " << jet.eta() << ", " << jet.phi() << ")" << std::endl;
    }

    auto wts = std::make_shared<std::vector<double>>();

    if constexpr (std::is_same<K, ProjectedEECCollection>::value){ //projected EEC
      auto dRs = std::make_shared<std::vector<double>>();
      auto coefs = std::make_shared<std::vector<std::vector<std::vector<double>>>>();
      auto tuplewts = std::make_shared<vecND<double>>(nConstituents, order_, 0);
      auto tupleiDR = std::make_shared<vecND<int>>(nConstituents, order_, -1);

      if(p1_==1 && p2_==1){
        projectedEEC(pt_, eta_, phi_, 
                     nConstituents, 2, 
                     *dRs, *wts, 
                     order_, 
                     coefs.get(), tuplewts.get(), tupleiDR.get());
      } else{
        EECnonIRC(pt_, eta_, phi_, nConstituents, p1_, p2_, *dRs, *wts);
      }
      if(std::accumulate(wts->begin(), wts->end(), 0.0) > 0){
        result->emplace_back(iJet, 
                             std::move(dRs), std::move(wts), 
                             order_, 
                             std::move(coefs), std::move(tuplewts), std::move(tupleiDR));
      }
    } else { //resolved EEC
      auto dRs = std::make_shared<std::vector<std::vector<double>>>();
      dRs->resize(choose(order_, 2));
      if(order_==3){
        full3ptEEC(pt_, eta_, phi_, nConstituents, 
            dRs->at(0), dRs->at(1), dRs->at(2), *wts);
      } else if(order_==4){
        full4ptEEC(pt_, eta_, phi_, nConstituents, 
            dRs->at(0), dRs->at(1), dRs->at(2),
            dRs->at(3), dRs->at(4), dRs->at(5),
            *wts);
      }
      if(std::accumulate(wts->begin(), wts->end(), 0.0) > 0){
        result->emplace_back(iJet, std::move(dRs), std::move(wts), order_);
      }
    } //end switch (EEC kind)
  }  // end for jet
  evt.put(std::move(result));
}  // end produce()

typedef EECProducerT<reco::PFJet, ProjectedEECCollection> ProjectedEECProducer;
typedef EECProducerT<reco::GenJet, ProjectedEECCollection> GenProjectedEECProducer;
typedef EECProducerT<pat::Jet, ProjectedEECCollection> PatProjectedEECProducer;

typedef EECProducerT<reco::PFJet, ResolvedEECCollection> ResolvedEECProducer;
typedef EECProducerT<reco::GenJet, ResolvedEECCollection> GenResolvedEECProducer;
typedef EECProducerT<pat::Jet, ResolvedEECCollection> PatResolvedEECProducer;

DEFINE_FWK_MODULE(ProjectedEECProducer);
DEFINE_FWK_MODULE(GenProjectedEECProducer);
DEFINE_FWK_MODULE(PatProjectedEECProducer);

DEFINE_FWK_MODULE(ResolvedEECProducer);
DEFINE_FWK_MODULE(GenResolvedEECProducer);
DEFINE_FWK_MODULE(PatResolvedEECProducer);
