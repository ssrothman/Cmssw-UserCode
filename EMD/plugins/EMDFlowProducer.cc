#include "SRothman/EMD/plugins/EMDFlowProducer.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "SRothman/DataFormats/interface/EMDFlow.h"

#define EPSILON 1e-15

EMDFlowProducer::EMDFlowProducer(const edm::ParameterSet& conf)
    : 
      jetsTag_(conf.getParameter<edm::InputTag>("jets")),
      jetsToken_(consumes<edm::View<pat::Jet>>(jetsTag_)),
      genJetsTag_(conf.getParameter<edm::InputTag>("genJets")),
      genJetsToken_(consumes<edm::View<reco::GenJet>>(genJetsTag_)),
      dR2cut_(conf.getParameter<double>("dR2cut")),
      minPartPt_(conf.getParameter<double>("minPartPt")),
      mode_(conf.getParameter<std::string>("mode")),
      emd_obj_(1.0, 2.0, true){
        //emd_obj_.preprocess<emd::CenterWeightedCentroid>();
        std::cout << emd_obj_.description() << std::endl;
        produces<EMDFlowCollection>();
}

void EMDFlowProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("jets");
  desc.add<edm::InputTag>("genJets");
  desc.add<double>("dR2cut");
  desc.add<double>("minPartPt");
  desc.add<std::string>("mode");
  descriptions.addWithDefaultLabel(desc);
}

void EMDFlowProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<pat::Jet>> jets;
  evt.getByToken(jetsToken_, jets);

  edm::Handle<edm::View<reco::GenJet>> genJets;
  evt.getByToken(genJetsToken_, genJets);

  auto result = std::make_unique<EMDFlowCollection>();
  
  std::vector<unsigned> taken;

  for(unsigned iReco=0; iReco<jets->size(); ++iReco){
    auto recoJet = jets->at(iReco);

    int bestGen=-1;
    double bestDR2=100000;

    for(unsigned iGen=0; iGen<genJets->size(); ++iGen){
      for(auto take : taken){
        if(take==iGen){
          continue;
        }
      }
      auto genJet = genJets->at(iGen);

      double dR2 = reco::deltaR2(recoJet.eta(), recoJet.phi(), genJet.eta(), genJet.phi());
      if(dR2>dR2cut_ || dR2>bestDR2){
        continue;
      } else {
        bestGen = iGen;
        bestDR2 = dR2;
      }
    }
    if(bestGen>=0){
      auto genJet = genJets->at(bestGen);

      jetConstituents reco;
      getConstituents_<pat::Jet>(recoJet, reco);

      jetConstituents gen;
      getConstituents_<reco::GenJet>(genJet, gen);

      unsigned NPReco = reco.size();
      unsigned NPGen = gen.size();

      printf("RECO %u   GEN %u\n\n", NPReco, NPGen);

      double emd = emd_obj_(reco, gen);
      auto flows = std::make_shared<std::vector<double>>(emd_obj_.flows());

      auto EG = std::make_shared<std::vector<double>>();
      auto ER = std::make_shared<std::vector<double>>();
      for(unsigned iPReco=0; iPReco<NPReco; ++iPReco){
        ER->push_back(reco[iPReco].weight());
      }
      for(unsigned iPGen=0; iPGen<NPGen; ++iPGen){
        EG->push_back(gen[iPGen].weight());
      }

      //printf("Matched (%0.3f, %0.3f, %0.3f) with (%0.3f, %0.3f, %0.3f)\n", 
      //    recoJet.pt(), recoJet.eta(), recoJet.phi(), 
      //    genJet.pt(), genJet.eta(), genJet.phi());
      //printf("\tEMD = %0.3f\n", emd);

      //flows indexed by (iPReco * nPGen + iPReco)??
      //printf("TOTAL SIZE = %zu (=%dx%d)\n", flows->size(), NPReco, NPGen);
      for(unsigned iPReco=0; iPReco<NPReco; ++iPReco){
        for(unsigned iPGen=0; iPGen<NPGen; ++iPGen){
          if(reco[iPReco].weight() > EPSILON){
            flows->at(iPReco*NPGen + iPGen) /= reco[iPReco].weight();
          } else{
            flows->at(iPReco*NPGen + iPGen) = 0;
          }
          printf("FLOW (%u, %u) = %0.3f\n", iPReco, iPGen, flows->at(iPReco*NPGen+iPGen));
          //printf("%0.3f\t", flows->at(iPReco*NPGen + iPGen));
        }
        //printf("\n");
      }
      //printf("\n");
      result->emplace_back(bestGen, iReco, std::move(flows), 
          NPGen, NPReco,
          std::move(EG), std::move(ER));
    } else{
      //printf("Matching failed for (%0.3f, %0.3f, %0.3f)\n",
      //    recoJet.pt(), recoJet.eta(), recoJet.phi()); 
    }
    //printf("\n");
  }
  evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(EMDFlowProducer);
