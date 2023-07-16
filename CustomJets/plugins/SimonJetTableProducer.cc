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

#include "SRothman/DataFormats/interface/jets.h"
#include "SRothman/DataFormats/interface/matching.h"

#include <iostream>
#include <memory>
#include <vector>

class SimonJetTableProducer : public edm::stream::EDProducer<> {
public:
    explicit SimonJetTableProducer(const edm::ParameterSet&);
    ~SimonJetTableProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    std::string name_;
    
    bool addMatch_;
    bool isGen_;
    edm::InputTag matchSrc_;
    edm::EDGetTokenT<edm::View<jetmatch>> matchToken_;
    edm::InputTag genJetSrc_;
    edm::EDGetTokenT<edm::View<jet>> genJetToken_;

    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<jet>> srcToken_;

    edm::InputTag otherMatchSrc_;
    edm::EDGetTokenT<edm::View<jetmatch>> otherMatchToken_;
    bool doOtherMatch_;

    int verbose_;
};

SimonJetTableProducer::SimonJetTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          addMatch_(conf.getParameter<bool>("addMatch")),
          isGen_(conf.getParameter<bool>("isGen")),
          matchSrc_(conf.getParameter<edm::InputTag>("matchSrc")),
          genJetSrc_(conf.getParameter<edm::InputTag>("genJets")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<jet>>(src_)),
          otherMatchSrc_(conf.getParameter<edm::InputTag>("otherMatchSrc")),
          doOtherMatch_(conf.getParameter<bool>("doOtherMatch")),
          verbose_(conf.getParameter<int>("verbose")){
    if(addMatch_){
      matchToken_ = consumes<edm::View<jetmatch>>(matchSrc_);
    }
    if(doOtherMatch_){
        otherMatchToken_ = consumes<edm::View<jetmatch>>(otherMatchSrc_);
    }
    if(addMatch_ && !isGen_){
        genJetToken_ = consumes<edm::View<jet>>(genJetSrc_);
    }
    produces<nanoaod::FlatTable>(name_);
    produces<nanoaod::FlatTable>(name_+"BK");
}

void SimonJetTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name");
  desc.add<bool>("addMatch");
  desc.add<bool>("isGen");
  desc.add<int>("verbose");
  desc.add<edm::InputTag>("src");
  desc.add<edm::InputTag>("matchSrc");
  desc.add<edm::InputTag>("otherMatchSrc");
  desc.add<edm::InputTag>("genJets");
  desc.add<bool>("doOtherMatch");
  descriptions.addWithDefaultLabel(desc);
}

void SimonJetTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of SimonJetTableProducer::produce()\n");
    }
  edm::Handle<edm::View<jet>> jets;
  evt.getByToken(srcToken_, jets);

  edm::Handle<edm::View<jetmatch>> matches;
  if(addMatch_){
      evt.getByToken(matchToken_, matches);
  }

  edm::Handle<edm::View<jetmatch>> otherMatches;
  if(doOtherMatch_){
      evt.getByToken(otherMatchToken_, otherMatches);
  }

  edm::Handle<edm::View<jet>> genJets;
  if(addMatch_ && !isGen_){
      evt.getByToken(genJetToken_, genJets);
  }

  std::vector<float>  partPt;
  std::vector<float> partEta;
  std::vector<float> partPhi;
  std::vector<int> pdgid;
  std::vector<int> charge;

  std::vector<int> nmatch;
  std::vector<float> matchPt, matchEta, matchPhi;

  std::vector<int> othernmatch;

  std::vector<float> pt;
  std::vector<float> eta;
  std::vector<float> phi;
  std::vector<int> iJet;
  std::vector<int> nPart;

  unsigned iJ=0;
  for(const auto& j : *jets){
      pt.push_back(j.pt);
      eta.push_back(j.eta);
      phi.push_back(j.phi);
      iJet.push_back(j.iJet);
      nPart.push_back(j.nPart);

      if(doOtherMatch_){
          std::vector<int> nextMatches;
          nextMatches.resize(j.particles.size(), 0);

          int matchidx = -1;
          for(unsigned iMatch=0; iMatch<matches->size(); ++iMatch){
              if(isGen_ && otherMatches->at(iMatch).iGen == iJ){
                  matchidx = iMatch;
                  break;
              } else if(!isGen_ && otherMatches->at(iMatch).iReco == iJ){
                  matchidx = iMatch;
                  break;
              }
          }
          if(matchidx >= 0){//if found match
              const auto& match = otherMatches->at(matchidx);
              for(unsigned i=0; i<match.ptrans.n_rows; ++i){//for i
                  for(unsigned j=0; j<match.ptrans.n_cols; ++j){//for j
                      if(match.ptrans(i, j) > 0){//if matched
                          if(isGen_){//if gen
                              nextMatches.at(j) += 1;
                          } else {//else if reco
                              nextMatches.at(i) += 1;
                          }//endif gen
                      }//end if matched
                  }//end for j
              }//end for i
          }//end if found match
          othernmatch.insert(othernmatch.end(), nextMatches.begin(),
                                                nextMatches.end());
      }

      if(addMatch_){//if doing matches
          std::vector<int> nextMatches;
          std::vector<float> nextPt, nextEta, nextPhi;
          nextMatches.resize(j.particles.size(), 0);
          nextPt.resize(j.particles.size(), 0);
          nextEta.resize(j.particles.size(), 0);
          nextPhi.resize(j.particles.size(), 0);

          int matchidx = -1;
          for(unsigned iMatch=0; iMatch<matches->size(); ++iMatch){
              if(isGen_ && matches->at(iMatch).iGen == iJ){
                  matchidx = iMatch;
                  break;
              } else if(!isGen_ && matches->at(iMatch).iReco == iJ){
                  matchidx = iMatch;
                  break;
              }
          }
          if(matchidx >=0 ){//if found match
              const auto& match = matches->at(matchidx);
              for(unsigned i=0; i<match.ptrans.n_rows; ++i){//for i
                  for(unsigned j=0; j<match.ptrans.n_cols; ++j){// for j
                      if(match.ptrans(i, j) > 0){//if matched
                          if(isGen_){//if gen
                              nextMatches.at(j) += 1;
                          } else {//else if reco
                              nextMatches.at(i) += 1;
                          }//endif (gen)
                      }//end if matched
                  }//end for j
              }//end for i

              if(!isGen_){
                  const auto& genj= genJets->at(match.iGen);
                  arma::vec genpt = genj.ptvec()/genj.sumpt;
                  arma::vec predpt = (match.ptrans*genpt);

                  arma::vec wgeneta = genpt % genj.etavec();
                  arma::vec wgenphi = genpt % genj.phivec();

                  arma::vec predeta = (match.ptrans*wgeneta)/predpt;
                  arma::vec predphi = (match.ptrans*wgenphi)/predpt;

                  predpt = predpt * j.sumpt;


                  for(unsigned i=0; i<j.nPart; ++i){
                      nextPt.at(i) = predpt(i);
                      nextEta.at(i) = predeta(i);
                      nextPhi.at(i) = predphi(i);
                  }
              }
          }// end if found match
          
          nmatch.insert(nmatch.end(), nextMatches.begin(), 
                                      nextMatches.end());
          matchPt.insert(matchPt.end(), nextPt.begin(), nextPt.end());
          matchEta.insert(matchEta.end(), nextEta.begin(), nextEta.end());
          matchPhi.insert(matchPhi.end(), nextPhi.begin(), nextPhi.end());
          
      }//end if doing matches

      for(const auto& p : j.particles){
          partPt.push_back(p.pt);
          partEta.push_back(p.eta);
          partPhi.push_back(p.phi);
          pdgid.push_back(p.pdgid);
          charge.push_back(p.charge);
      }
      ++iJ;
  }
    if(verbose_){
        printf("filled vectors\n");
    }

  auto table = std::make_unique<nanoaod::FlatTable>(partPt.size(), name_, false);
  table->addColumn<float>("pt", partPt, "particle pt", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("eta", partEta, "particle eta", nanoaod::FlatTable::FloatColumn);
  table->addColumn<float>("phi", partPhi, "particle phi", nanoaod::FlatTable::FloatColumn);
  table->addColumn<int>("pdgid", pdgid, "particle pdgid", nanoaod::FlatTable::IntColumn);
  table->addColumn<int>("charge", charge, "particle charge", nanoaod::FlatTable::IntColumn);
  if(addMatch_){
    table->addColumn<int>("nmatch", nmatch, "number of particle matches", nanoaod::FlatTable::IntColumn);
  }
  if(doOtherMatch_){
      table->addColumn<int>("onmatch", othernmatch, "number of particle matches [other]", nanoaod::FlatTable::IntColumn);
  }
  if(addMatch_ && !isGen_){
      table->addColumn<float>("matchPt", matchPt, "predicted particle pt", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("matchEta", matchEta, "predicted particle eta", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("matchPhi", matchPhi, "predicted particle phi", nanoaod::FlatTable::FloatColumn);
  }
  evt.put(std::move(table), name_);
  if(verbose_){
    printf("made table with %lu elements\n", partPt.size());
  }

  auto tableBK = std::make_unique<nanoaod::FlatTable>(pt.size(), name_+"BK", false);
  tableBK->addColumn<float>("jetPt", pt, "jet pt", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<float>("jetEta", eta, "jet eta", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<float>("jetPhi", phi, "jet phi", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<int>("iJet", iJet, "index in primary jet array", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nPart", nPart, "number of particles in jet", nanoaod::FlatTable::IntColumn);
  evt.put(std::move(tableBK), name_+"BK");
  if(verbose_){
    printf("made tableBK with %lu elements\n", pt.size());
  }
}

DEFINE_FWK_MODULE(SimonJetTableProducer);
