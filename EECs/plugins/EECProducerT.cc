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

#include "SRothman/EECs/src/eec.h"
#include "SRothman/EECs/src/combinatorics.h"
#include "SRothman/EECs/src/vecND.h"

#include "SRothman/DataFormats/interface/EEC.h"

#include <iostream>
#include <memory>
#include <vector>

#define VERBOSE 

template <typename K>
class EECProducerT : public edm::stream::EDProducer<> {
public:
  explicit EECProducerT(const edm::ParameterSet&);
  ~EECProducerT() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  unsigned int order_;
  double minPartPt_;

  unsigned int p1_, p2_;

  edm::InputTag src_;
  edm::EDGetTokenT<EECPartsCollection> srcToken_;
};

template <typename K>
EECProducerT<K>::EECProducerT(const edm::ParameterSet& conf)
    : order_(conf.getParameter<unsigned int>("order")),
      minPartPt_(conf.getParameter<double>("minPartPt")),
      p1_(conf.getParameter<unsigned int>("p1")),
      p2_(conf.getParameter<unsigned int>("p2")),
      src_(conf.getParameter<edm::InputTag>("EECParts")),
      srcToken_(consumes<EECPartsCollection>(src_)){
  produces<K>();

  if( (p1_!=1 || p2_!=1) && order_!=2)
    throw cms::Exception("EECProducer") << "Only 2-point nonIRC EECs are supported at the moment" << std::endl;
}

template <typename K>
void EECProducerT<K>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<unsigned int>("order");
  desc.add<double>("minPartPt");
  desc.add<edm::InputTag>("EECParts");
  desc.add<unsigned int>("p1");
  desc.add<unsigned int>("p2");
  descriptions.addWithDefaultLabel(desc);
}

template <typename K>
void EECProducerT<K>::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<EECPartsCollection> partsvec;
  evt.getByToken(srcToken_, partsvec);

  auto result = std::make_unique<K>();

  for (const auto& parts : *partsvec) {

    size_t nConstituents = parts.partPt->size();

    auto wts = std::make_shared<std::vector<double>>();

    std::vector<double> normPt;
    for(const double pt : *(parts.partPt)){
      normPt.push_back(pt/parts.rawPt);
    }

    if constexpr (std::is_same<K, ProjectedEECCollection>::value){ //projected EEC
      auto dRs = std::make_shared<std::vector<double>>();
      auto coefs = std::make_shared<std::vector<std::vector<std::vector<double>>>>();
      auto tuplewts = std::make_shared<vecND<double>>(nConstituents, order_, 0);
      auto tupleiDR = std::make_shared<vecND<int>>(nConstituents, order_, -1);

      if(p1_==1 && p2_==1){
        projectedEEC(normPt.data(), parts.partEta->data(), parts.partPhi->data(), 
                     nConstituents, 2, 
                     *dRs, *wts, 
                     order_, 
                     coefs.get(), tuplewts.get(), tupleiDR.get());
      } else{
        EECnonIRC(normPt.data(), parts.partEta->data(), parts.partPhi->data(),
            nConstituents, p1_, p2_, *dRs, *wts);
      }
      result->emplace_back(parts.iJet, 
                           std::move(dRs), std::move(wts), 
                           order_, 
                           std::move(coefs), std::move(tuplewts), std::move(tupleiDR));
    } else { //resolved EEC
      auto dRs = std::make_shared<std::vector<std::vector<double>>>();
      dRs->resize(choose(order_, 2));
      if(order_==3){
        full3ptEEC(normPt.data(), parts.partEta->data(), parts.partPhi->data(),
            nConstituents, 
            dRs->at(0), dRs->at(1), dRs->at(2), *wts);
      } else if(order_==4){
        full4ptEEC(normPt.data(), parts.partEta->data(), parts.partPhi->data(),
            nConstituents, 
            dRs->at(0), dRs->at(1), dRs->at(2),
            dRs->at(3), dRs->at(4), dRs->at(5),
            *wts);
      }
      result->emplace_back(parts.iJet, std::move(dRs), std::move(wts), order_);
    } //end switch (EEC kind)
  }  // end for jet
  evt.put(std::move(result));
}  // end produce()

typedef EECProducerT<ProjectedEECCollection> ProjectedEECProducer;
typedef EECProducerT<ResolvedEECCollection> ResolvedEECProducer;

DEFINE_FWK_MODULE(ProjectedEECProducer);
DEFINE_FWK_MODULE(ResolvedEECProducer);
