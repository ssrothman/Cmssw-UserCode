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
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "SRothman/SimonTools/src/jet.h"
#include "SRothman/SimonTools/src/util.h"

#include "SRothman/CustomJets/plugins/AddParticle.h"
#include "SRothman/SimonTools/src/isID.h"
#include "SRothman/SimonTools/src/particleThresholds.h"
#include "SRothman/SimonTools/src/partSyst.h"
#include "SRothman/SimonTools/src/computeJetMass.h"

#include <iostream>
#include <memory>
#include <vector>
#include <random>

template <typename T>
class UniformGaussianJetProducerT : public edm::stream::EDProducer<> {
public:
    explicit UniformGaussianJetProducerT(const edm::ParameterSet&);
    ~UniformGaussianJetProducerT() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    simon::partSyst systematics_;
    simon::partSyst::SYSTEMATIC syst_;

    bool passPtEtaPhi(const T& jet);
    bool passLepVeto(const T& jet);
    
    struct simon::particleThresholds thresholds_;
    struct simon::vtxCuts vtxcuts_;

    bool onlyCharged_;

    unsigned int maxNumPart_, minNumPart_;
    
    bool applyJEC_;
    bool applyPuppi_;

    double minPt_;
    double maxEta_;

    double maxMuFrac_;
    double maxChEmFrac_;

    edm::InputTag jetSrc_;
    edm::EDGetTokenT<edm::View<T>> jetSrcToken_;

    edm::InputTag CHSsrc_;
    edm::EDGetTokenT<edm::View<T>> CHSsrcToken_;
    bool addCHSindex_;
    double CHSmatchDR_;

    edm::InputTag evtSelSrc_;
    edm::EDGetTokenT<bool> evtSelToken_;
    bool doEvtSel_;

    int verbose_;

    double radius_;
    std::default_random_engine generator_;
    std::normal_distribution<double> distribution_;
};

template <typename T>
UniformGaussianJetProducerT<T>::UniformGaussianJetProducerT(const edm::ParameterSet& conf)
        : systematics_(conf.getParameter<edm::ParameterSet>("systematics")),
          syst_(simon::partSyst::getSystEnum(conf.getParameter<std::string>("syst"))),
          thresholds_(conf.getParameter<edm::ParameterSet>("thresholds")),
          vtxcuts_(conf.getParameter<edm::ParameterSet>("vtxCuts")),
          onlyCharged_(conf.getParameter<bool>("onlyCharged")),
          maxNumPart_(conf.getParameter<unsigned>("maxNumPart")),
          minNumPart_(conf.getParameter<unsigned>("minNumPart")),
          minPt_(conf.getParameter<double>("minPt")),
          maxEta_(conf.getParameter<double>("maxEta")),
          maxMuFrac_(conf.getParameter<double>("maxMuFrac")),
          maxChEmFrac_(conf.getParameter<double>("maxChEmFrac")),
          jetSrc_(conf.getParameter<edm::InputTag>("jetSrc")),
          jetSrcToken_(consumes<edm::View<T>>(jetSrc_)),
          CHSsrc_(conf.getParameter<edm::InputTag>("CHSsrc")),
          CHSsrcToken_(consumes<edm::View<T>>(CHSsrc_)),
          addCHSindex_(conf.getParameter<bool>("addCHSindex")),
          CHSmatchDR_(conf.getParameter<double>("CHSmatchDR")),
          evtSelSrc_(conf.getParameter<edm::InputTag>("eventSelection")),
          evtSelToken_(consumes<bool>(evtSelSrc_)),
          doEvtSel_(conf.getParameter<bool>("doEventSelection")),
          verbose_(conf.getParameter<int>("verbose")),
          radius_(conf.getParameter<double>("radius")),
          generator_(std::random_device()()),
          distribution_(0.0, radius_){
    produces<std::vector<simon::jet>>();
}

template <typename T>
void UniformGaussianJetProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<std::string>("syst");

  edm::ParameterSetDescription systPset;
  simon::partSyst::fillPSetDescription(systPset);
  desc.add<edm::ParameterSetDescription>(
            "systematics", systPset);

  edm::ParameterSetDescription thresholdPset;
  simon::particleThresholds::fillPSetDescription(thresholdPset);
  desc.add<edm::ParameterSetDescription>(
            "thresholds", thresholdPset);


  desc.add<bool>("onlyCharged");

  edm::ParameterSetDescription vtxCutsPset;
  simon::vtxCuts::fillPSetDescription(vtxCutsPset);
  desc.add<edm::ParameterSetDescription>(
            "vtxCuts", vtxCutsPset);

  desc.add<unsigned>("maxNumPart");
  desc.add<unsigned>("minNumPart");

  desc.add<double>("minPt");
  desc.add<double>("maxEta");
  desc.add<double>("maxMuFrac");
  desc.add<double>("maxChEmFrac");

  desc.add<edm::InputTag>("eventSelection");
  desc.add<bool>("doEventSelection");

  desc.add<edm::InputTag>("jetSrc");
  desc.add<edm::InputTag>("CHSsrc");
  desc.add<bool>("addCHSindex");
  desc.add<double>("CHSmatchDR");

  desc.add<int>("verbose");

  desc.add<bool>("applyJEC");
  desc.add<bool>("applyPuppi");

  desc.add<double>("radius");

  descriptions.addWithDefaultLabel(desc);
}

template <typename T>
bool UniformGaussianJetProducerT<T>::passPtEtaPhi(const T& jet){
    return jet.pt() > minPt_ && std::fabs(jet.eta()) < maxEta_;
}

template <typename T>
bool UniformGaussianJetProducerT<T>::passLepVeto(const T& jet){
    double chEmFrac=0;
    double muFrac=0;

    if constexpr(std::is_same<T, pat::Jet>::value){
        chEmFrac = jet.chargedEmEnergyFraction();
        muFrac = jet.muonEnergyFraction();
    } else if constexpr(std::is_same<T, reco::GenJet>::value){
        chEmFrac = jet.chargedEmEnergy() / jet.energy();
        muFrac = jet.muonEnergy() / jet.energy();
    } else {
        throw std::logic_error("UniformGaussianJetProducer: unknown jet type");
    }

    return (chEmFrac < maxChEmFrac_) && (muFrac < maxMuFrac_);
}

template <typename T>
void UniformGaussianJetProducerT<T>::produce(edm::Event& evt, 
                                   const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of UniformGaussianJetProducerT<T>::produce()\n");
    }
    edm::Handle<edm::View<T>> jets;
    evt.getByToken(jetSrcToken_, jets);

    edm::Handle<edm::View<T>> CHSjets;
    if(addCHSindex_){
        evt.getByToken(CHSsrcToken_, CHSjets);
    }

    auto result = std::make_unique<std::vector<simon::jet>>();

    if(doEvtSel_){
        edm::Handle<bool> evtSel;
        evt.getByToken(evtSelToken_, evtSel);
        if(!*evtSel){
            evt.put(std::move(result));
            return;
        }
    }

    if(verbose_){
        printf("passed event selection\n");
    }

    for(unsigned iJet=0; iJet < jets->size(); ++iJet){//for each jet
        const auto& j = jets->at(iJet);

        if(!passPtEtaPhi(j) || !passLepVeto(j)){
            continue;
        }
        
        const auto& constituents = j.getJetConstituents();

        if(constituents.size() < minNumPart_){
            continue;
        }

        if(verbose_){
            printf("jet %d passed jet selection\n", iJet);
        }

        double pt = j.pt();
        double eta = j.eta();
        double phi = j.phi();
        double rawpt=pt;
        double jecfactor=1;
        if constexpr(std::is_same<T, pat::Jet>::value){
            jecfactor = j.jecFactor("Uncorrected");
            rawpt = pt * jecfactor; 
        } else if constexpr(std::is_same<T, reco::GenJet>::value){
            jecfactor = 1;
            rawpt = pt;
        } else {
            throw std::logic_error("UniformGaussianJetProducer: unknown jet type");
        }

        simon::jet ans;
        ans.pt = pt;
        ans.eta = eta;
        ans.phi = phi;
        ans.iJet = iJet;
        ans.jecfactor = jecfactor;

        //printf("making jSimonJet et with eta: %f, phi: %f\n", ans.eta, ans.phi);

        if(addCHSindex_){
            for(unsigned iCHS=0; iCHS < CHSjets->size(); ++iCHS){
                const auto& jCHS = CHSjets->at(iCHS);
                if(deltaR(j, jCHS) < CHSmatchDR_){//if the CHS jet matches the jet
                    ans.iCHS.push_back(iCHS);
                }
            }
            if(ans.iCHS.empty()){
                ans.iCHS.push_back(99999999);
            }
        }
  
        if (verbose_){
            printf("\tjet: (%f, %f, %f)\n", pt, eta, phi);
        }

        for(const auto& part : constituents){//for each constituent
            if(verbose_>1){
                printf("part: (%f, %f, %f, %f)\n", part->pt(),
                                               part->eta(), 
                                               part->phi(),
                                               part->mass());
            }
            const auto* partptr = dynamic_cast<const pat::PackedCandidate*>(part.get());
            const auto* genptr = dynamic_cast<const pat::PackedGenParticle*>(part.get());
            const auto* genptr2 = dynamic_cast<const reco::GenParticle*>(part.get());
            
            if(partptr){
                addParticle(partptr, ans, jecfactor, 
                            applyPuppi_, applyJEC_, 
                            onlyCharged_,
                            9999, thresholds_,
                            vtxcuts_, systematics_, syst_,
                            maxNumPart_);
           } else if(genptr){
                addParticle(genptr, ans, jecfactor, 
                            applyPuppi_, applyJEC_,
                            onlyCharged_,
                            9999, thresholds_,
                            vtxcuts_, systematics_, simon::partSyst::NOM,
                            maxNumPart_);
           } else if(genptr2){
               addParticle(genptr2, ans, jecfactor, 
                            applyPuppi_, applyJEC_,
                            onlyCharged_,
                            9999, thresholds_,
                            vtxcuts_, systematics_, simon::partSyst::NOM,
                            maxNumPart_);
           } else {
                throw std::runtime_error("constituent is not a PackedCandidate or PackedGenCandidate or GenParticle");
            }
        } // end for part

        if(verbose_){
            printf("rawPt = %f\n", rawpt);
            printf("pt = %f\n", pt);
            printf("sumpt = %f\n", ans.sumpt);
            printf("jec = %f\n", jecfactor);
        }

        for (auto& part : ans.particles){
            part.eta = ans.eta + distribution_(generator_);
            part.phi = ans.phi + distribution_(generator_);
        }

        computeJetMass(ans);

        if(ans.nPart >= minNumPart_){
            result->push_back(std::move(ans));
        }

        if(verbose_){
            printf("pushed back\n");
        }
    }  // end for jet
    evt.put(std::move(result));
    if(verbose_){
        printf("put into event\n");
    }
}  // end produce()

typedef UniformGaussianJetProducerT<pat::Jet> PatUniformGaussianJetProducer;
typedef UniformGaussianJetProducerT<reco::GenJet> GenUniformGaussianJetProducer;

DEFINE_FWK_MODULE(PatUniformGaussianJetProducer);
DEFINE_FWK_MODULE(GenUniformGaussianJetProducer);
