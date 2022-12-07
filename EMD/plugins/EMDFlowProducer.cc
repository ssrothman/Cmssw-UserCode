#include "SRothman/EMD/plugins/EMDFlowProducer.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "SRothman/DataFormats/interface/EMDFlow.h"

#include "SRothman/armadillo/include/armadillo"

#define EPSILON 1e-15

EMDFlowProducer::EMDFlowProducer(const edm::ParameterSet& conf)
    : 
      jetsTag_(conf.getParameter<edm::InputTag>("jets")),
      jetsToken_(consumes<edm::View<pat::Jet>>(jetsTag_)),
      genJetsTag_(conf.getParameter<edm::InputTag>("genJets")),
      genJetsToken_(consumes<edm::View<reco::GenJet>>(genJetsTag_)),
      dR2cut_(conf.getParameter<double>("dR2cut")),
      minPartPt_(conf.getParameter<double>("minPartPt")),
      partDR2cut_(conf.getParameter<double>("partDR2cut")),
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
  desc.add<double>("partDR2cut");
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
      double recoFactor = getConstituents_<pat::Jet>(recoJet, reco);

      jetConstituents gen;
      double genFactor = getConstituents_<reco::GenJet>(genJet, gen);

      unsigned NPReco = reco.size();
      unsigned NPGen = gen.size();

      auto flowmat = std::make_shared<arma::mat>(reco.size(), gen.size(), arma::fill::zeros);

      if(mode_ == "Ewt" || mode_ == "nowt"){
        double emd = emd_obj_(gen, reco);
        std::vector<double> flows = emd_obj_.flows();

        for(unsigned iPReco=0; iPReco<NPReco; ++iPReco){
            for(unsigned iPGen=0; iPGen<NPGen; ++iPGen){
              if(gen[iPGen].weight() > EPSILON){
                (*flowmat)(iPReco, iPGen) = flows.at(iPGen*NPReco + iPReco)
                                            * recoFactor / gen[iPGen].weight();
              } else{
                (*flowmat)(iPReco, iPGen) = 0;
              }
            }
          }
      } else if(mode_ == "match"){
        //vector of matched gen particles for each reco particle
        std::vector<std::vector<unsigned>> matched;
        matched.resize(NPReco);

        for(unsigned iPGen=0; iPGen < NPGen; ++iPGen){//for each gen particle
          const EMDParticle& genPart = gen[iPGen];
          for(unsigned iPReco=0; iPReco < NPReco; ++iPReco){//for each reco particle
            const EMDParticle& recoPart = reco[iPReco];

            double dR2 = reco::deltaR2(recoPart[0], recoPart[1], genPart[0], genPart[1]);
            if(dR2 < partDR2cut_){ //reco particle too far away from gen particle
              matched[iPReco].emplace_back(iPGen);
            }
          }//end for each reco particle
        }//end for each gen particle

        for(unsigned iPReco=0; iPReco<NPReco; ++iPReco){//for each reco particle
          const EMDParticle& recoPart = reco[iPReco];
          if(matched[iPReco].size()==0){//matching failed. No entry in flow matrix
            continue;
          } else if(matched[iPReco].size()==1){//matching is obvious. 
                                               //This should be the most common case?
            if(gen[matched[iPReco][0]].weight() > EPSILON){
              (*flowmat)(iPReco, matched[iPReco][0]) 
                = reco[iPReco].weight()/gen[matched[iPReco][0]].weight();
            }
          } else{//have to determine how much to give each gen particle from the reco particle
                 //for the moment lets do a naive energy weighting
            double matchedWt=0;
            //std::cout << "Nmatch: " << std::endl;
            for(const auto idx : matched[iPReco]){//for each matched gen particle
              matchedWt += gen[idx].weight();
            }//end for each matched gen particle
            for(const auto idx : matched[iPReco]){//for each matched gen particle (again)
              if(gen[idx].weight()>EPSILON){
                (*flowmat)(iPReco, idx) = (gen[idx].weight()/matchedWt) 
                                          * reco[iPReco].weight()/gen[idx].weight(); 
              }
              //printf("\t(%u, %u) = %0.3f\n", idx, iPReco, recoPart.weight()*gen[idx].weight()/matchedWt);
            }//end for each matched gen particle
          }//end switch(NMatches)
        }//end for each reco particle
      } else {
        throw cms::Exception("EMDFlowProducer") << "Unsupported mode" << std::endl;
      }//end switch(mode_)

      auto EG = std::make_shared<std::vector<double>>();
      auto ER = std::make_shared<std::vector<double>>();
      for(unsigned iPReco=0; iPReco<NPReco; ++iPReco){
        ER->push_back(reco[iPReco].weight());
      }
      for(unsigned iPGen=0; iPGen<NPGen; ++iPGen){
        EG->push_back(gen[iPGen].weight());
      }

      arma::vec genvec(gen.size(), arma::fill::zeros);
      for(unsigned i=0; i<gen.size(); ++i){
        genvec(i) = gen[i].weight();
      }
      arma::vec recovec(reco.size(), arma::fill::zeros);
      for(unsigned i=0; i<reco.size(); ++i){
        recovec(i) = reco[i].weight();
      }

      result->emplace_back(bestGen, iReco, std::move(flowmat), 
          NPGen, NPReco,
          std::move(EG), std::move(ER),
          nullptr, nullptr);
    }
  }
  evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(EMDFlowProducer);
