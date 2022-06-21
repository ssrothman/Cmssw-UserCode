#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"
#include "RecoVertex/VertexPrimitives/interface/VertexState.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"

#include "RecoBTag/FeatureTools/interface/TrackInfoBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"

#include "DataFormats/BTauReco/interface/TrackIPTagInfo.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
#include "RecoBTag/FeatureTools/interface/deep_helpers.h"
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
using namespace btagbtvdeep;

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"

class EECTableProducer : public edm::stream::EDProducer<> {
public:
  explicit EECTableProducer(const edm::ParameterSet &);
  ~EECTableProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  void produce(edm::Event &, const edm::EventSetup &) override;

  //const std::string name_;
  const std::string name_;
  
  edm::EDGetTokenT<std::vector<float>> dRsT_, wtsT_;
  edm::EDGetTokenT<std::vector<unsigned>> nDRsT_;

  edm::Handle<std::vector<float>> dRs_, wts_;
  edm::Handle<std::vector<unsigned>> nDRs_;  
};

//
// constructors and destructor
//
EECTableProducer::EECTableProducer(const edm::ParameterSet &iConfig)
    : name_(iConfig.getParameter<std::string>("name")),
      dRsT_(consumes<std::vector<float>>(iConfig.getParameter<edm::InputTag>("dRs"))),
      wtsT_(consumes<std::vector<float>>(iConfig.getParameter<edm::InputTag>("wts"))),
      nDRsT_(consumes<std::vector<unsigned>>(iConfig.getParameter<edm::InputTag>("nDRs"))){
  produces<nanoaod::FlatTable>(name_);
}

EECTableProducer::~EECTableProducer() {}

void EECTableProducer::produce(edm::Event &iEvent, const edm::EventSetup &iSetup) {
  // elements in all these collections must have the same order!
  // PF Cands
  iEvent.getByToken(dRsT_, dRs_);
  iEvent.getByToken(wtsT_, wts_);
  iEvent.getByToken(nDRsT_, nDRs_);

  std::vector<int> jetIdx(dRs_->size());

  for(unsigned i=0; i<nDRs_->size(); ++i){
    for(unsigned j=0; j<nDRs_->at(i); ++j){
      jetIdx.push_back(i);
    }
  }

  auto table = std::make_unique<nanoaod::FlatTable>(dRs_->size(), name_, false);
  // std::cout << "DEBUG : candTable (" << name_ << ") has N = " << outCands->size() << std::endl;
  // We fill from here only stuff that cannot be created with the SimpleFlatTableProducer
  table->addColumn<float>("dRs", *dRs_, "Delta R", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("wts", *wts_, "Weight", nanoaod::FlatTable::FloatColumn);
  table->addColumn<int>("jetIdx", jetIdx, "jet iundex", nanoaod::FlatTable::IntColumn);

  iEvent.put(std::move(table), name_);
}

void EECTableProducer::fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name", "EECs");
  desc.add<edm::InputTag>("dRs", edm::InputTag("dR2"));
  desc.add<edm::InputTag>("wts", edm::InputTag("wt2"));
  desc.add<edm::InputTag>("nDRs", edm::InputTag("nDR"));
  descriptions.addWithDefaultLabel(desc);
}

DEFINE_FWK_MODULE(EECTableProducer);
