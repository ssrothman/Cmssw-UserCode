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

#include "SRothman/Matching/src/simon_util_cpp/deltaR.h"
#include "SRothman/Matching/src/simon_util_cpp/util.h"

#include "SRothman/Matching/src/matcher.h"

#include <iostream>
#include <memory>
#include <vector>


class GenMatchProducer : public edm::stream::EDProducer<> {
public:
    explicit GenMatchProducer(const edm::ParameterSet&);
    ~GenMatchProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    int verbose_;
    double dR2thresh_;
    
    double clipval_, cutoff_;
    bool matchCharge_;
    unsigned maxReFit_;

    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;
    edm::InputTag genTag_;
    edm::EDGetTokenT<edm::View<jet>> genToken_;
};

GenMatchProducer::GenMatchProducer(const edm::ParameterSet& conf)
        : verbose_(conf.getParameter<int>("verbose")),
          dR2thresh_(square(conf.getParameter<double>("dRthresh"))),
          clipval_(conf.getParameter<double>("clipval")),
          cutoff_(conf.getParameter<double>("cutoff")),
          matchCharge_(conf.getParameter<bool>("matchCharge")),
          maxReFit_(conf.getParameter<unsigned>("maxReFit")),
          recoTag_(conf.getParameter<edm::InputTag>("reco")),
          recoToken_(consumes<edm::View<jet>>(recoTag_)),
          genTag_(conf.getParameter<edm::InputTag>("gen")),
          genToken_(consumes<edm::View<jet>>(genTag_)){
    produces<std::vector<jetmatch>>();
}

void GenMatchProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<double>("dRthresh");
  desc.add<double>("clipval");
  desc.add<double>("cutoff");
  desc.add<bool>("matchCharge");
  desc.add<unsigned>("maxReFit");
  desc.add<edm::InputTag>("reco");
  desc.add<edm::InputTag>("gen");
  desc.add<int>("verbose");
  descriptions.addWithDefaultLabel(desc);
}

void GenMatchProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<jet>> reco;
  evt.getByToken(recoToken_, reco);

  edm::Handle<edm::View<jet>> gen;
  evt.getByToken(recoToken_, gen);

  auto result = std::make_unique<std::vector<jetmatch>>();

  std::vector<bool> taken(gen->size(), false);
  for(unsigned iReco=0; iReco<reco->size(); ++iReco){
      const jet& jreco = reco->at(iReco);
      for(unsigned iGen=0; iGen<gen->size(); ++iGen){
          if(taken[iGen]){
              continue;
          }
          const jet& jgen = gen->at(iGen);
    
          double dist = dR2(jreco.eta, jreco.phi, 
                            jgen.eta, jgen.phi);
          if(dist > dR2thresh_){
              continue;
          }

          jetmatch next;
          next.iReco = iReco;
          next.iGen = iGen;

          matcher match (jreco.particles, jgen.particles,
                         clipval_, cutoff_, matchCharge_, 
                         maxReFit_);
          match.minimize();
          next.ptrans = match.ptrans();

          result->push_back(std::move(next));
      }
  }
  evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(GenMatchProducer);
