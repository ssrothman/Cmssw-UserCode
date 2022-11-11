#include "DataFormats/Math/interface/deltaR.h"
#include "SRothman/DataFormats/interface/EMDFlow.h"

#include "SRothman/armadillo/include/armadillo"


#include "SRothman/GenMatchFit/plugins/GenMatchFitProducer.h"
#include "SRothman/GenMatchFit/plugins/GenMatchFCN.h"

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MnContours.h"
#include "Minuit2/MnPlot.h"

#define EPSILON 1e-15

using namespace ROOT::Minuit2;

// requires at least C++11
//from https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf/49812018#49812018
static const std::string vformat(const char * const zcFormat, ...) {

  // initialize use of the variable argument array
  va_list vaArgs;
  va_start(vaArgs, zcFormat);

  // reliably acquire the size
  // from a copy of the variable argument array
  // and a functionally reliable call to mock the formatting
  va_list vaArgsCopy;
  va_copy(vaArgsCopy, vaArgs);
  const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
  va_end(vaArgsCopy);

  // return a formatted string without risking memory mismanagement
  // and without assuming any compiler or platform specific behavior
  std::vector<char> zc(iLen + 1);
  std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
  va_end(vaArgs);
  return std::string(zc.data(), iLen); 
}

GenMatchFitProducer::GenMatchFitProducer(const edm::ParameterSet& conf)
    : 
      jetsTag_(conf.getParameter<edm::InputTag>("jets")),
      jetsToken_(consumes<edm::View<pat::Jet>>(jetsTag_)),
      genJetsTag_(conf.getParameter<edm::InputTag>("genJets")),
      genJetsToken_(consumes<edm::View<reco::GenJet>>(genJetsTag_)),
      dR2cut_(conf.getParameter<double>("dR2cut")),
      minPartPt_(conf.getParameter<double>("minPartPt")),
      partDR2cut_(conf.getParameter<double>("partDR2cut")),
      recoPT_(std::make_shared<std::vector<double>>()),
      recoETA_(std::make_shared<std::vector<double>>()),
      recoPHI_(std::make_shared<std::vector<double>>()),
      genPT_(std::make_shared<std::vector<double>>()),
      genETA_(std::make_shared<std::vector<double>>()),
      genPHI_(std::make_shared<std::vector<double>>()),
      errPT_(std::make_shared<std::vector<double>>()),
      errETA_(std::make_shared<std::vector<double>>()),
      errPHI_(std::make_shared<std::vector<double>>()),
      maxIter_(conf.getParameter<unsigned>("maxIter")),
      feasCondition_(conf.getParameter<double>("feasCondition")),
      startMu_(conf.getParameter<double>("startMu")),
      startLambda_(conf.getParameter<double>("startLambda")),
      clipVal_(conf.getParameter<double>("clipVal"))
{

        produces<EMDFlowCollection>();
}

void GenMatchFitProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("jets");
  desc.add<edm::InputTag>("genJets");
  desc.add<double>("dR2cut");
  desc.add<double>("minPartPt");
  desc.add<double>("partDR2cut");
  desc.add<unsigned>("maxIter");
  desc.add<double>("feasCondition");
  desc.add<double>("startMu");
  desc.add<double>("startLambda");
  desc.add<double>("clipVal");
  descriptions.addWithDefaultLabel(desc);
}

void GenMatchFitProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
  edm::Handle<edm::View<pat::Jet>> jets;
  evt.getByToken(jetsToken_, jets);

  edm::Handle<edm::View<reco::GenJet>> genJets;
  evt.getByToken(genJetsToken_, genJets);

  auto result = std::make_unique<EMDFlowCollection>();
  
  std::vector<unsigned> taken;

  for(unsigned iReco=0; iReco<jets->size(); ++iReco){//for each reco jet
    const auto& recoJet = jets->at(iReco);

    int bestGen=-1;
    double bestDR2=100000;

    for(unsigned iGen=0; iGen<genJets->size(); ++iGen){//for each gen jet
      for(auto take : taken){//check if already matched
        if(take==iGen){
          continue;
        }
      }//end check if already matched

      const auto& genJet = genJets->at(iGen);

      double dR2 = reco::deltaR2(recoJet.eta(), recoJet.phi(), genJet.eta(), genJet.phi());
      if(dR2>dR2cut_ || dR2>bestDR2){//update best match
        continue;
      } else {
        bestGen = iGen;
        bestDR2 = dR2;
      }
    }//end for each gen jet

    if(bestGen>=0){//if matched
      const auto& genJet = genJets->at(bestGen);

      double recoFactor = getConstituents_<pat::Jet>(recoJet, recoPT_, recoETA_, recoPHI_);

      double genFactor = getConstituents_<reco::GenJet>(genJet, genPT_, genETA_, genPHI_);

      unsigned NPReco = recoPT_->size();
      unsigned NPGen = genPT_->size();

      GenMatchFCN FCN(recoPT_, recoETA_, recoPHI_,
                      genPT_, genETA_, genPHI_,
                      recoPT_, recoETA_, recoPHI_, 
                      3.0, 1.0,
                      startMu_, startLambda_);

      MnUserParameters upar;
      for(size_t iReco=0; iReco<recoPT_->size(); ++iReco){
        for(size_t iGen=0; iGen<genPT_->size(); ++iGen){

          double dR2 = reco::deltaR2(recoETA_->at(iReco), recoPHI_->at(iReco),
                                     genETA_->at(iGen), genPHI_->at(iGen));

          std::string name = vformat("%lux%lu", iReco, iGen);
          if(dR2 < partDR2cut_){
            upar.Add(name, 0.5, 0.5); 
            upar.SetLowerLimit(name, 0.0);
          } else{
            upar.Add(name, 0., 0.);
            upar.Fix(name);
          }
        }
      }

      size_t iIter=0;
      bool converged=false;
      double feas=0;
      MnMigrad *migrad = new MnMigrad(FCN, upar);
      FunctionMinimum min = (*migrad)();
      while(iIter++<maxIter_ && !converged){
        std::cout << "iter " << iIter << std::endl;
        min = (*migrad)();
        std::cout << "minimized" << std::endl;

        auto state = min.UserState();
        std::cout << "got state" << std::endl;

        feas = FCN.updateLambdas(state.Parameters().Params());
        FCN.updateMu(2.0); 
        std::cout << "updated FCN" << std::endl;
  
        //std::cout << "minimum: " << min << std::endl;
        std::cout << "feas: " << feas << std::endl;

        if(feas < feasCondition_){
          std::cout << "evaluating feasibility of condition" << std::endl;
          converged=true;
          break;
        }

        std::cout << "failed feas condition, going back around the loop" << std::endl;
        delete migrad;
        migrad = new MnMigrad(FCN, state, MnStrategy(2));
        std::cout << "remade migrad" << std::endl;
      }
  
      std::vector<double> C(genPT_->size(), 0);
      for(size_t iReco=0; iReco<recoPT_->size(); ++iReco){
        for(size_t iGen=0; iGen<genPT_->size(); ++iGen){
          std::string name = vformat("%lux%lu", iReco, iGen);
          C.at(iGen) += migrad->Value(name.c_str());
        }
      }

      for(size_t iReco=0; iReco<recoPT_->size(); ++iReco){
        for(size_t iGen=0; iGen<genPT_->size(); ++iGen){
          std::string name = vformat("%lux%lu", iReco, iGen);
          if(C.at(iGen)==0){
            migrad->SetValue(name.c_str(), 0);
            migrad->Fix(name.c_str());
          } else {
            double val = migrad->Value(name.c_str())/C.at(iGen);
            if(val < clipVal_){
              migrad->SetValue(name.c_str(), 0);
              migrad->Fix(name.c_str());
            } else if (1-val < clipVal_){
              migrad->SetValue(name.c_str(), 1.0);
              migrad->Fix(name.c_str());
            }
          }
        }
      }
      min = (*migrad)();
      //std::cout << "FINAL OPTIMUM" << min << std::endl;
            

      std::vector<double> C2(genPT_->size(), 0);
      for(size_t iReco=0; iReco<recoPT_->size(); ++iReco){
        for(size_t iGen=0; iGen<genPT_->size(); ++iGen){
          std::string name = vformat("%lux%lu", iReco, iGen);
          C2.at(iGen) += migrad->Value(name.c_str());
        }
      }

      auto flowmat = std::make_shared<arma::mat>(NPReco, NPGen, arma::fill::zeros);
      for(size_t iReco=0; iReco<recoPT_->size(); ++iReco){
        for(size_t iGen=0; iGen<genPT_->size(); ++iGen){
          std::string name = vformat("%lux%lu", iReco, iGen);
          (*flowmat)(iReco, iGen) = migrad->Value(name.c_str())/C2.at(iGen);
        }
      }

      std::cout << "Flow matrix" << std::endl << *flowmat << std::endl;

      delete migrad;
      migrad = nullptr;

      auto EG = std::make_shared<std::vector<double>>();
      auto ER = std::make_shared<std::vector<double>>();
      for(unsigned iPReco=0; iPReco<NPReco; ++iPReco){
        ER->push_back(recoPT_->at(iPReco));
      }
      for(unsigned iPGen=0; iPGen<NPGen; ++iPGen){
        EG->push_back(genPT_->at(iPGen));
      }

      result->emplace_back(bestGen, iReco, std::move(flowmat), 
          NPGen, NPReco,
          std::move(EG), std::move(ER));
    }//end if matched
  }//end for each reco jet
  evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(GenMatchFitProducer);
