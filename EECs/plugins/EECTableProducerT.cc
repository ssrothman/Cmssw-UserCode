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

#include <iostream>
#include <memory>
#include <vector>

#define MAX_CONSTITUENTS 256

#define VERBOSE 

enum EECkind { Projected, Full3Pt, Full4Pt };

template <typename T, EECkind K>
class EECTableProducerT : public edm::stream::EDProducer<> {
public:
  explicit EECTableProducerT(const edm::ParameterSet&);
  ~EECTableProducerT() override {}
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  void produce(edm::Event&, const edm::EventSetup&) override;

private:
  unsigned int order_;
  double minJetPt_;

  std::string name_;

  unsigned int verbose_;
  unsigned int p1_, p2_;

  edm::InputTag src_;
  edm::EDGetTokenT<edm::View<T>> srcToken_;

  edm::InputTag muonSrc_;
  edm::EDGetTokenT<edm::View<reco::Muon>> muonSrcToken_;

  float pt_[MAX_CONSTITUENTS], eta_[MAX_CONSTITUENTS], phi_[MAX_CONSTITUENTS];

};

template <typename T, EECkind K>
EECTableProducerT<T, K>::EECTableProducerT(const edm::ParameterSet& conf)
    : order_(conf.getParameter<unsigned int>("order")),
      minJetPt_(conf.getParameter<double>("minJetPt")),
      name_(conf.getParameter<std::string>("name")),
      verbose_(conf.getParameter<unsigned int>("verbose")),
      p1_(conf.getParameter<unsigned int>("p1")),
      p2_(conf.getParameter<unsigned int>("p2")),
      src_(conf.getParameter<edm::InputTag>("jets")),
      srcToken_(consumes<edm::View<T>>(src_)),
      muonSrc_(conf.getParameter<edm::InputTag>("muons")),
      muonSrcToken_(consumes<edm::View<reco::Muon>>(muonSrc_)){
  produces<nanoaod::FlatTable>(name_);
  if( (p1_!=1 || p2_!=1) && order_!=2)
    throw cms::Exception("EECTableProducer") << "Only 2-point nonIRC EECs are supported at the moment" << std::endl;
}

template <typename T, EECkind K>
void EECTableProducerT<T, K>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<unsigned int>("order");
  desc.add<std::string>("name");
  desc.add<double>("minJetPt");
  desc.add<edm::InputTag>("jets");
  desc.add<edm::InputTag>("muons");
  desc.add<unsigned int>("verbose");
  desc.add<unsigned int>("p1");
  desc.add<unsigned int>("p2");
  descriptions.addWithDefaultLabel(desc);
}

template <typename T, EECkind K>
void EECTableProducerT<T, K>::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<T>> jets;
  evt.getByToken(srcToken_, jets);

  edm::Handle<edm::View<reco::Muon>> muons;
  evt.getByToken(muonSrcToken_, muons);

  unsigned nJets = jets->size();

  if(verbose_)
    std::cout << name_ << " Event has " << nJets << " jets" << std::endl;
  if (muons->size() == 2) {
    if(verbose_)
      std::cout << name_ << "\tEvent has two muons... accepted" << std::endl;
    if constexpr(K==EECkind::Projected){
      auto flatDRs = std::make_unique<std::vector<float>>();
      auto flatWTs = std::make_unique<std::vector<float>>();
      auto jetIdx = std::make_unique<std::vector<int>>();
      for (size_t iJet = 0; iJet < nJets; ++iJet) {
        std::vector<float> dRs, wts;

        T jet = jets->at(iJet);

        std::vector<reco::Jet::Constituent> constituents = jet.getJetConstituents();
        size_t nConstituents = std::min<size_t>(constituents.size(), MAX_CONSTITUENTS);

        if (jet.pt() < minJetPt_ || nConstituents<order_){ //skip jet
          dRs.push_back(0);
          wts.push_back(0);
        } else { //don't skip jet
        
          if (verbose_)
            std::cout << name_ << "\tjet: (" << jet.pt() << ", " << jet.eta() << ", " << jet.phi() << ")" << std::endl;


          for (size_t i = 0; i < nConstituents; ++i) {
            auto part = constituents[i];
            pt_[i] = (float)part->pt() / jet.pt();
            eta_[i] = (float)part->eta();
            phi_[i] = (float)part->phi();
            if (verbose_)
              std::cout << name_ << "\t\tpart: (" << part->pt() << ", " << part->eta() << ", " << part->phi() << ")" << std::endl;
          }

          if(p1_==1 && p2_==1){
            projectedEEC(pt_, eta_, phi_, nConstituents, order_, 2, dRs, wts);
          } else{
            EECnonIRC(pt_, eta_, phi_, nConstituents, p1_, p2_, dRs, wts);
          }
        } //end if not skipping jet

        flatDRs->insert(flatDRs->end(), dRs.begin(), dRs.end());
        flatWTs->insert(flatWTs->end(), wts.begin(), wts.end());
        jetIdx->insert(jetIdx->end(), dRs.size(), iJet);
      }  // end for jet
      auto table = std::make_unique<nanoaod::FlatTable>(flatDRs->size(), name_, false);
      table->addColumn<float>("dRs", *flatDRs, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("wts", *flatWTs, "Weight", nanoaod::FlatTable::FloatColumn);
      table->addColumn<int>("jetIdx", *jetIdx, "jet index", nanoaod::FlatTable::IntColumn);
      evt.put(std::move(table), name_);
    } else if constexpr(K==EECkind::Full3Pt){
      auto flatDR1 = std::make_unique<std::vector<float>>();
      auto flatDR2 = std::make_unique<std::vector<float>>();
      auto flatDR3 = std::make_unique<std::vector<float>>();
      auto flatWTs = std::make_unique<std::vector<float>>();
      auto jetIdx = std::make_unique<std::vector<int>>();

      for (size_t iJet = 0; iJet < nJets; ++iJet) {
        std::vector<float> dR1, dR2, dR3, wts;
        T jet = jets->at(iJet);

        std::vector<reco::Jet::Constituent> constituents = jet.getJetConstituents();
        size_t nConstituents = std::min<size_t>(constituents.size(), MAX_CONSTITUENTS);

        if (jet.pt() < minJetPt_ || nConstituents<3){
          dR1.push_back(0);
          dR2.push_back(0);
          dR3.push_back(0);
          wts.push_back(0);
        } else {
          for (size_t i = 0; i < nConstituents; ++i) {
            auto part = constituents[i];
            pt_[i] = (float)part->pt() / jet.pt();
            eta_[i] = (float)part->eta();
            phi_[i] = (float)part->phi();
          }


          full3ptEEC(pt_, eta_, phi_, nConstituents, dR1, dR2, dR3, wts);
        }

        flatDR1->insert(flatDR1->end(), dR1.begin(), dR1.end());
        flatDR2->insert(flatDR2->end(), dR2.begin(), dR2.end());
        flatDR3->insert(flatDR3->end(), dR3.begin(), dR3.end());
        flatWTs->insert(flatWTs->end(), wts.begin(), wts.end());
        jetIdx->insert(jetIdx->end(), dR1.size(), iJet);
      }  // end for jet
      auto table = std::make_unique<nanoaod::FlatTable>(flatDR1->size(), name_, false);
      table->addColumn<float>("dR1", *flatDR1, "Longest Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR2", *flatDR2, "Second Longest Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR3", *flatDR3, "Third Longest Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("wts", *flatWTs, "Weight", nanoaod::FlatTable::FloatColumn);
      table->addColumn<int>("jetIdx", *jetIdx, "jet index", nanoaod::FlatTable::IntColumn);
      evt.put(std::move(table), name_);
    } else {//if K == Full4Pt
      auto flatDR1 = std::make_unique<std::vector<float>>();
      auto flatDR2 = std::make_unique<std::vector<float>>();
      auto flatDR3 = std::make_unique<std::vector<float>>();
      auto flatDR4 = std::make_unique<std::vector<float>>();
      auto flatDR5 = std::make_unique<std::vector<float>>();
      auto flatDR6 = std::make_unique<std::vector<float>>();
      auto flatWTs = std::make_unique<std::vector<float>>();
      auto jetIdx = std::make_unique<std::vector<int>>();

      for (size_t iJet = 0; iJet < nJets; ++iJet) {
        std::vector<float> dR1, dR2, dR3, dR4, dR5, dR6, wts;

        T jet = jets->at(iJet);

        std::vector<reco::Jet::Constituent> constituents = jet.getJetConstituents();
        size_t nConstituents = std::min<size_t>(constituents.size(), MAX_CONSTITUENTS);

        if (jet.pt() < minJetPt_ || nConstituents<4){
          dR1.push_back(0);
          dR2.push_back(0);
          dR3.push_back(0);
          dR4.push_back(0);
          dR5.push_back(0);
          dR6.push_back(0);
          wts.push_back(0);
        } else {
          for (size_t i = 0; i < nConstituents; ++i) {
            auto part = constituents[i];
            pt_[i] = (float)part->pt() / jet.pt();
            eta_[i] = (float)part->eta();
            phi_[i] = (float)part->phi();
          }


          full4ptEEC(pt_, eta_, phi_, nConstituents, dR1, dR2, dR3, dR4, dR5, dR6, wts);
        }

        flatDR1->insert(flatDR1->end(), dR1.begin(), dR1.end());
        flatDR2->insert(flatDR2->end(), dR2.begin(), dR2.end());
        flatDR3->insert(flatDR3->end(), dR3.begin(), dR3.end());
        flatDR4->insert(flatDR4->end(), dR4.begin(), dR4.end());
        flatDR5->insert(flatDR5->end(), dR5.begin(), dR5.end());
        flatDR6->insert(flatDR6->end(), dR6.begin(), dR6.end());
        flatWTs->insert(flatWTs->end(), wts.begin(), wts.end());
        jetIdx->insert(jetIdx->end(), dR1.size(), iJet);
      }  // end for jet
      auto table = std::make_unique<nanoaod::FlatTable>(flatDR1->size(), name_, false);
      table->addColumn<float>("dR1", *flatDR1, "Longest Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR2", *flatDR2, "Second Longest Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR3", *flatDR3, "Third Longest Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR4", *flatDR4, "Fourth Longest Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR5", *flatDR5, "Fifth Longest Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR6", *flatDR6, "Sixth Longest Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("wts", *flatWTs, "Weight", nanoaod::FlatTable::FloatColumn);
      table->addColumn<int>("jetIdx", *jetIdx, "jet index", nanoaod::FlatTable::IntColumn);
      evt.put(std::move(table), name_);
    }
  } else {  //there are not two muons
    auto table = std::make_unique<nanoaod::FlatTable>(0, name_, false);
    std::vector<float> emptyFloat;
    std::vector<int> emptyInt;
    if constexpr(K==EECkind::Projected){
      table->addColumn<float>("dRs", emptyFloat, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("wts", emptyFloat, "Weight", nanoaod::FlatTable::FloatColumn);
      table->addColumn<int>("jetIdx", emptyInt, "jet index", nanoaod::FlatTable::IntColumn);
    } else if constexpr(K==EECkind::Full3Pt){
      table->addColumn<float>("dR1", emptyFloat, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR2", emptyFloat, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR3", emptyFloat, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("wts", emptyFloat, "Weight", nanoaod::FlatTable::FloatColumn);
      table->addColumn<int>("jetIdx", emptyInt, "jet index", nanoaod::FlatTable::IntColumn);
    } else{
      table->addColumn<float>("dR1", emptyFloat, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR2", emptyFloat, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR3", emptyFloat, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR4", emptyFloat, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR5", emptyFloat, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("dR6", emptyFloat, "Delta R", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("wts", emptyFloat, "Weight", nanoaod::FlatTable::FloatColumn);
      table->addColumn<int>("jetIdx", emptyInt, "jet index", nanoaod::FlatTable::IntColumn);
    }

    evt.put(std::move(table), name_);
  }
}  // end produce()

typedef EECTableProducerT<reco::PFJet, EECkind::Projected> ProjectedEECTableProducer;
typedef EECTableProducerT<reco::GenJet, EECkind::Projected> GenProjectedEECTableProducer;
typedef EECTableProducerT<pat::Jet, EECkind::Projected> PatProjectedEECTableProducer;

typedef EECTableProducerT<reco::PFJet, EECkind::Full3Pt> Full3PtEECTableProducer;
typedef EECTableProducerT<reco::GenJet, EECkind::Full3Pt> GenFull3PtEECTableProducer;
typedef EECTableProducerT<pat::Jet, EECkind::Full3Pt> PatFull3PtEECTableProducer;

typedef EECTableProducerT<reco::PFJet, EECkind::Full4Pt> Full4PtEECTableProducer;
typedef EECTableProducerT<reco::GenJet, EECkind::Full4Pt> GenFull4PtEECTableProducer;
typedef EECTableProducerT<pat::Jet, EECkind::Full4Pt> PatFull4PtEECTableProducer;

DEFINE_FWK_MODULE(ProjectedEECTableProducer);
DEFINE_FWK_MODULE(GenProjectedEECTableProducer);
DEFINE_FWK_MODULE(PatProjectedEECTableProducer);

DEFINE_FWK_MODULE(Full3PtEECTableProducer);
DEFINE_FWK_MODULE(GenFull3PtEECTableProducer);
DEFINE_FWK_MODULE(PatFull3PtEECTableProducer);

DEFINE_FWK_MODULE(Full4PtEECTableProducer);
DEFINE_FWK_MODULE(GenFull4PtEECTableProducer);
DEFINE_FWK_MODULE(PatFull4PtEECTableProducer);
