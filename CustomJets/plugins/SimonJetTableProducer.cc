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
          verbose_(conf.getParameter<int>("verbose")){
    if(addMatch_){
      matchToken_ = consumes<edm::View<jetmatch>>(matchSrc_);
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
  desc.add<edm::InputTag>("genJets");
  descriptions.addWithDefaultLabel(desc);
}

void SimonJetTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of SimonJetTableProducer::produce()\n");
    }
    //std::cout << "The name is " << name_ << std::endl;
  edm::Handle<edm::View<jet>> jets;
  evt.getByToken(srcToken_, jets);

  edm::Handle<edm::View<jetmatch>> matches;
  if(addMatch_){
      evt.getByToken(matchToken_, matches);
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
  std::vector<int> nmatchMuon, nmatchEle, nmatchEM0, nmatchHAD0, nmatchHADCH;

  std::vector<float> pt;
  std::vector<float> rawPt;
  std::vector<float> eta;
  std::vector<float> phi;
  std::vector<int> iJet;
  std::vector<int> nPart;

  std::vector<float> genPt;
  std::vector<float> genEta;
  std::vector<float> genPhi;

  unsigned iJ=0;
  for(const auto& j : *jets){
      pt.push_back(j.pt);
      rawPt.push_back(j.sumpt);
      eta.push_back(j.eta);
      phi.push_back(j.phi);
      iJet.push_back(j.iJet);
      nPart.push_back(j.nPart);

      if(addMatch_){//if doing matches
          std::vector<int> nextMatches, nextMatchMuon, nextMatchEle, nextMatchEM0, nextMatchHAD0, nextMatchHADCH;
          std::vector<float> nextPt, nextEta, nextPhi;
          nextMatches.resize(j.particles.size(), 0);
          nextMatchMuon.resize(j.particles.size(), 0);
          nextMatchEle.resize(j.particles.size(), 0);
          nextMatchEM0.resize(j.particles.size(), 0);
          nextMatchHAD0.resize(j.particles.size(), 0);
          nextMatchHADCH.resize(j.particles.size(), 0);
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
              for(unsigned i=0; i<match.rawmat.n_rows; ++i){//for i
                  for(unsigned j=0; j<match.rawmat.n_cols; ++j){// for j
                      if(match.rawmat(i, j) > 0){//if matched
                          unsigned idx = isGen_ ? j : i;
                          nextMatches.at(idx) += 1;
                          if (!isGen_){
                              unsigned pdgId = genJets->at(match.iGen).particles.at(j).pdgid;
                              int charge = genJets->at(match.iGen).particles.at(j).charge;
                              if(pdgId == 13){
                                  nextMatchMuon.at(idx) += 1;
                              } else if(pdgId == 11){
                                  nextMatchEle.at(idx) += 1;
                              } else if(pdgId == 22){
                                  nextMatchEM0.at(idx) += 1;
                              } else if(charge == 0){
                                  nextMatchHAD0.at(idx) += 1;
                              } else {
                                  nextMatchHADCH.at(idx) += 1;
                              }
                          }
                      }//end if matched
                  }//end for j
              }//end for i

              if(!isGen_){
                  const auto& genj= genJets->at(match.iGen);
                  
                  arma::vec genpt = genj.ptvec();
                  arma::vec recopt = j.ptvec();

                  arma::vec predpt = (match.rawmat*genpt);

                  arma::vec wgeneta = genpt % genj.etavec();
                  arma::vec wgenphi = genpt % genj.phivec();

                  arma::vec predeta = (match.rawmat*wgeneta)/predpt;
                  arma::vec predphi = (match.rawmat*wgenphi)/predpt;

                  for(unsigned i=0; i<j.nPart; ++i){
                      nextPt.at(i) = predpt(i);
                      nextEta.at(i) = predeta(i);
                      nextPhi.at(i) = predphi(i);
                  }

                  genPt.push_back(genj.pt);
                  genEta.push_back(genj.eta);
                  genPhi.push_back(genj.phi);
              }
          } else if(!isGen_){
              genPt.push_back(-1);
              genEta.push_back(-1);
              genPhi.push_back(-1);
          }// end if found match
          
          nmatch.insert(nmatch.end(), nextMatches.begin(), 
                                      nextMatches.end());
          nmatchMuon.insert(nmatchMuon.end(), nextMatchMuon.begin(),
                                              nextMatchMuon.end());
          nmatchEle.insert(nmatchEle.end(), nextMatchEle.begin(),
                                            nextMatchEle.end());
          nmatchEM0.insert(nmatchEM0.end(), nextMatchEM0.begin(),
                                            nextMatchEM0.end());
          nmatchHAD0.insert(nmatchHAD0.end(), nextMatchHAD0.begin(),
                                              nextMatchHAD0.end());
          nmatchHADCH.insert(nmatchHADCH.end(), nextMatchHADCH.begin(),
                                                nextMatchHADCH.end());
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
  if(addMatch_ && !isGen_){
      table->addColumn<float>("matchPt", matchPt, "predicted particle pt", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("matchEta", matchEta, "predicted particle eta", nanoaod::FlatTable::FloatColumn);
      table->addColumn<float>("matchPhi", matchPhi, "predicted particle phi", nanoaod::FlatTable::FloatColumn);
      table->addColumn<int>("nmatchMuon", nmatchMuon, "number of muon matches", nanoaod::FlatTable::IntColumn);
      table->addColumn<int>("nmatchEle", nmatchEle, "number of electron matches", nanoaod::FlatTable::IntColumn);
      table->addColumn<int>("nmatchEM0", nmatchEM0, "number of photon matches", nanoaod::FlatTable::IntColumn);
      table->addColumn<int>("nmatchHAD0", nmatchHAD0, "number of neutral hadron matches", nanoaod::FlatTable::IntColumn);
      table->addColumn<int>("nmatchHADCH", nmatchHADCH, "number of charged hadron matches", nanoaod::FlatTable::IntColumn);
  }
  evt.put(std::move(table), name_);
  if(verbose_){
    printf("made table with %lu elements\n", partPt.size());
  }

  auto tableBK = std::make_unique<nanoaod::FlatTable>(pt.size(), name_+"BK", false);
  tableBK->addColumn<float>("jetPt", pt, "jet pt", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<float>("jetRawPt", rawPt, "raw jet pt", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<float>("jetEta", eta, "jet eta", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<float>("jetPhi", phi, "jet phi", nanoaod::FlatTable::FloatColumn);
  tableBK->addColumn<int>("iJet", iJet, "index in primary jet array", nanoaod::FlatTable::IntColumn);
  tableBK->addColumn<int>("nPart", nPart, "number of particles in jet", nanoaod::FlatTable::IntColumn);
  if(addMatch_ && !isGen_){
      tableBK->addColumn<float>("genPt", genPt, "gen jet pt", nanoaod::FlatTable::FloatColumn);
      tableBK->addColumn<float>("genEta", genEta, "gen jet eta", nanoaod::FlatTable::FloatColumn);
      tableBK->addColumn<float>("genPhi", genPhi, "gen jet phi", nanoaod::FlatTable::FloatColumn);
  }
  evt.put(std::move(tableBK), name_+"BK");
  if(verbose_){
    printf("made tableBK with %lu elements\n", pt.size());
  }
}

DEFINE_FWK_MODULE(SimonJetTableProducer);
