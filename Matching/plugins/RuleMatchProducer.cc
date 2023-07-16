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

#include "SRothman/SimonTools/src/deltaR.h"
#include "SRothman/SimonTools/src/util.h"

#include "SRothman/Matching/src/matcher.h"

#include <iostream>
#include <memory>
#include <vector>

class RuleMatchProducer : public edm::stream::EDProducer<> {
public:
    explicit RuleMatchProducer(const edm::ParameterSet&);
    ~RuleMatchProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    int verbose_;

    double jetMatchingDR2_;

    double partMatchingDR2_;

    double pTtoleranceEM0_;
    double pTtoleranceHAD0_;
    double pTtoleranceCH_;
    
    bool allowManyEM0_;
    bool allowManyHAD0_;
    bool allowManyCH_;

    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;
    edm::InputTag genTag_;
    edm::EDGetTokenT<edm::View<jet>> genToken_;
};

RuleMatchProducer::RuleMatchProducer(const edm::ParameterSet& conf)
        : verbose_(conf.getParameter<int>("verbose")),
          jetMatchingDR2_(square(conf.getParameter<double>("jetMatchingDR"))),
          partMatchingDR2_(square(conf.getParameter<double>("partMatchingDR"))),
          pTtoleranceEM0_(conf.getParameter<double>("pTtoleranceEM0")),
          pTtoleranceHAD0_(conf.getParameter<double>("pTtoleranceHAD0")),
          pTtoleranceCH_(conf.getParameter<double>("pTtoleranceCH")),
          allowManyEM0_(conf.getParameter<bool>("allowManyEM0")),
          allowManyHAD0_(conf.getParameter<bool>("allowManyHAD0")),
          allowManyCH_(conf.getParameter<bool>("allowManyCH")),
          recoTag_(conf.getParameter<edm::InputTag>("reco")),
          recoToken_(consumes<edm::View<jet>>(recoTag_)),
          genTag_(conf.getParameter<edm::InputTag>("gen")),
          genToken_(consumes<edm::View<jet>>(genTag_)){

    produces<std::vector<jetmatch>>();
}

void RuleMatchProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<double>("jetMatchingDR");

  desc.add<double>("partMatchingDR");

  desc.add<double>("pTtoleranceEM0");
  desc.add<double>("pTtoleranceHAD0");
  desc.add<double>("pTtoleranceCH");

  desc.add<bool>("allowManyEM0");
  desc.add<bool>("allowManyHAD0");
  desc.add<bool>("allowManyCH");

  desc.add<edm::InputTag>("reco");
  desc.add<edm::InputTag>("gen");
  desc.add<int>("verbose");
  descriptions.addWithDefaultLabel(desc);
}

static void printParticle(const particle& part){
    printf("pt: %f, eta: %f, phi: %f, pdgId: %d, charge: %d\n",
           part.pt, part.eta, part.phi, part.pdgid, part.charge);
}
static void printJet(const jet& jet){
    for(const auto& part : jet.particles){
        printParticle(part);
    }
}

void RuleMatchProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  if(verbose_){
    printf("Top of RuleMatchProducer::produce()\n");
  }
  edm::Handle<edm::View<jet>> reco;
  evt.getByToken(recoToken_, reco);

  edm::Handle<edm::View<jet>> gen;
  evt.getByToken(genToken_, gen);

  auto result = std::make_unique<std::vector<jetmatch>>();

  std::vector<bool> taken(gen->size(), false);
  for(unsigned iReco=0; iReco<reco->size(); ++iReco){//for each reco jet
      if(verbose_){
          printf("doing jet %u\n", iReco);
      }
      const jet& jreco = reco->at(iReco);
      int matchedgen = -1;
      for(unsigned iGen=0; iGen<gen->size(); ++iGen){//for each gen jet
          if(taken[iGen]){
              continue;
          }
          const jet& jgen = gen->at(iGen);
    
          double dist = dR2(jreco.eta, jreco.phi, 
                            jgen.eta, jgen.phi);
          if(dist > jetMatchingDR2_){
              continue;
          }

          matchedgen = iGen;
          taken[iGen] = true;
          break;
      }//end for each gen

      if(matchedgen >= 0){//if matched
          if(verbose_){
              printf("matched with %u\n", matchedgen);
          }
          const jet& jgen = gen->at(matchedgen);

          struct jetmatch next;
          next.iReco = iReco;
          next.iGen = matchedgen;
          next.ptrans = arma::mat(jreco.particles.size(), jgen.particles.size(), arma::fill::zeros);

          if(verbose_){
              printf("\nRECO JET\n");
              printJet(jreco);
              printf("\nGEN JET\n");
              printJet(jgen);
          }

          std::vector<bool> genTaken(jgen.particles.size(), false);

          for(unsigned iReco=0; iReco<jreco.particles.size(); ++iReco){//for each reco particle
              bool hasMatch = false;
              double accuPt = 0;
              for(unsigned iGen=0; iGen<jgen.particles.size(); ++iGen){//for each gen particle
                  if(verbose_>1)
                      printf("doing gen %u, reco %u\n", iGen, iReco);
                  if(genTaken[iGen]){
                      if(verbose_>1)
                          printf("\tskipping because gen taken\n");
                      continue;
                  }

                  const particle& preco = jreco.particles[iReco];
                  const particle& pgen = jgen.particles[iGen];

                  if(hasMatch){//if already matched
                      if(preco.pdgid == 22 && !allowManyEM0_){
                          if(verbose_>1)
                              printf("\tskipping because reco already matched (EM0)\n");
                          continue;
                      } else if(preco.pdgid == 130 && !allowManyHAD0_){
                          if(verbose_>1)
                              printf("\tskipping because reco already matched (HAD0)\n");
                          continue;
                      } else if(preco.charge != 0 && !allowManyCH_){
                          if(verbose_>1)
                              printf("\tskipping because reco already matched (CH)\n");
                          continue;
                      }
                  }//end if already matched

                  double dist = dR2(preco.eta, preco.phi, 
                                    pgen.eta, pgen.phi);
                  if(dist > partMatchingDR2_){//if no match
                      if(verbose_>1)
                          printf("\tdR matching failed\n");
                      continue;
                  } else{//else if match
                      if(hasMatch){//else, check if match actually helps
                          double currentdiff = std::abs(accuPt - preco.pt);
                          double newdiff = std::abs(accuPt + pgen.pt - preco.pt);
                          if(currentdiff < newdiff){
                              if(verbose_>1)
                                  printf("\tmatching makes things worse\n");
                              continue;
                          }
                      }

                    genTaken[iGen] = true;
                    hasMatch = true;
                    accuPt += pgen.pt;
                    next.ptrans(iReco, iGen) = 1;
                    if(verbose_>1)
                        printf("\tmatched reco particle %u with gen particle %u\n", iReco, iGen);

                  }//end if match
              }//end for each reco part
          }//end for each gen part
          arma::vec pTpred = next.ptrans * jgen.ptvec();
          for(unsigned iReco=0; iReco<jreco.particles.size(); ++iReco){//for each reco particle
              const particle& preco = jreco.particles[iReco];
              double pTerr = std::abs(preco.pt - pTpred(iReco))/preco.pt;
              if(verbose_>1)
                  printf("pTerr: %f\n", pTerr);
              if(preco.pdgid == 22){//if EM0
                  if(pTerr > pTtoleranceEM0_ && pTtoleranceEM0_ > 0){
                      if(verbose_>1)
                          printf("EM0 pT tolerance failed\n");
                      next.ptrans.row(iReco).fill(0);
                  }
              } else if(preco.pdgid == 130){//if HAD0
                  if(pTerr > pTtoleranceHAD0_ && pTtoleranceHAD0_ > 0){
                      if(verbose_>1)
                          printf("HAD0 pT tolerance failed\n");
                      next.ptrans.row(iReco).fill(0);
                  }
              } else if(preco.charge != 0){//if CH
                  if(pTerr > pTtoleranceCH_ && pTtoleranceCH_ > 0){
                      if(verbose_>1)
                          printf("CH pT tolerance failed\n");
                      next.ptrans.row(iReco).fill(0);
                  }
              } else {
                  throw cms::Exception("LogicError", "particle is not EM0, HAD0, or CH");
              }
          }//end for each reco part
          result->push_back(next);
      }//end if matched
  }//end for each reco jet

  evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(RuleMatchProducer);
