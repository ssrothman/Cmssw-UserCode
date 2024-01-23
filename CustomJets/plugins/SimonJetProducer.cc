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

#include "SRothman/SimonTools/src/jets.h"
#include "SRothman/SimonTools/src/util.h"

#include "SRothman/CustomJets/plugins/AddParticle.h"
#include "SRothman/SimonTools/src/isID.h"

#include <iostream>
#include <memory>
#include <vector>

template <typename T>
class SimonJetProducerT : public edm::stream::EDProducer<> {
public:
    explicit SimonJetProducerT(const edm::ParameterSet&);
    ~SimonJetProducerT() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    bool passPtEtaPhi(const T& jet);
    bool passLepVeto(const T& jet);
    
    double EM0threshold_, HAD0threshold_;
    double HADCHthreshold_, ELEthreshold_, MUthreshold_;

    unsigned int maxNumPart_, minNumPart_;

    int verbose_;

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

    bool applyJEC_;
    bool applyPuppi_;
    bool rescaleMiniAODtruncation_;
};

template <typename T>
SimonJetProducerT<T>::SimonJetProducerT(const edm::ParameterSet& conf)
        : EM0threshold_(conf.getParameter<double>("EM0threshold")),
          HAD0threshold_(conf.getParameter<double>("HAD0threshold")),
          HADCHthreshold_(conf.getParameter<double>("HADCHthreshold")),
          ELEthreshold_(conf.getParameter<double>("ELEthreshold")),
          MUthreshold_(conf.getParameter<double>("MUthreshold")),
          maxNumPart_(conf.getParameter<unsigned>("maxNumPart")),
          minNumPart_(conf.getParameter<unsigned>("minNumPart")),
          verbose_(conf.getParameter<int>("verbose")),
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
          applyJEC_(conf.getParameter<bool>("applyJEC")),
          applyPuppi_(conf.getParameter<bool>("applyPuppi")),
          rescaleMiniAODtruncation_(conf.getParameter<bool>("rescaleMiniAODtruncation")){
    produces<std::vector<jet>>();
}

template <typename T>
void SimonJetProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<double>("EM0threshold");
  desc.add<double>("HAD0threshold");
  desc.add<double>("HADCHthreshold");
  desc.add<double>("MUthreshold");
  desc.add<double>("ELEthreshold");

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
  desc.add<bool>("rescaleMiniAODtruncation");

  descriptions.addWithDefaultLabel(desc);
}

template <typename T>
bool SimonJetProducerT<T>::passPtEtaPhi(const T& jet){
    return jet.pt() > minPt_ && std::fabs(jet.eta()) < maxEta_;
}

template <typename T>
bool SimonJetProducerT<T>::passLepVeto(const T& jet){
    double chEmFrac=0;
    double muFrac=0;

    if constexpr(std::is_same<T, pat::Jet>::value){
        chEmFrac = jet.chargedEmEnergyFraction();
        muFrac = jet.muonEnergyFraction();
    } else if constexpr(std::is_same<T, reco::GenJet>::value){
        chEmFrac = jet.chargedEmEnergy() / jet.energy();
        muFrac = jet.muonEnergy() / jet.energy();
    } else {
        throw std::logic_error("SimonJetProducer: unknown jet type");
    }

    return (chEmFrac < maxChEmFrac_) && (muFrac < maxMuFrac_);
}

template <typename T>
void SimonJetProducerT<T>::produce(edm::Event& evt, 
                                   const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of SimonJetProducerT<T>::produce()\n");
    }
    edm::Handle<edm::View<T>> jets;
    evt.getByToken(jetSrcToken_, jets);

    edm::Handle<edm::View<T>> CHSjets;
    if(addCHSindex_){
        evt.getByToken(CHSsrcToken_, CHSjets);
    }

    auto result = std::make_unique<std::vector<jet>>();

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
            throw std::logic_error("SimonJetProducer: unknown jet type");
        }

        jet ans;
        ans.pt = pt;
        ans.eta = eta;
        ans.phi = phi;
        ans.iJet = iJet;
        ans.jecfactor = jecfactor;

        if(addCHSindex_){
            for(unsigned iCHS=0; iCHS < CHSjets->size(); ++iCHS){
                const auto& jCHS = CHSjets->at(iCHS);
                if(deltaR(j, jCHS) < CHSmatchDR_){//if the CHS jet matches the jet
                    ans.iCHS = iCHS;
                    break;
                }
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
            
            double minpt;
            if(isEM0(part)){
                minpt = EM0threshold_;
            } else if(isHAD0(part)){
                minpt = HAD0threshold_;
            } else if(isHADCH(part)){
                minpt = HADCHthreshold_;
            } else if(isELE(part)){
                minpt = ELEthreshold_;
            } else if(isMU(part)){
                minpt = MUthreshold_;
            } else{
                throw std::runtime_error("constituent is not EM0, HAD0, HADCH, ELE, or MU");
            }

            if(partptr){
                addParticle(partptr, ans, jecfactor, 
                            applyPuppi_, applyJEC_,
                            minpt, 9999,
                            maxNumPart_);
           } else if(genptr){
                addParticle(genptr, ans, jecfactor, 
                            applyPuppi_, applyJEC_,
                            minpt, 9999,
                            maxNumPart_);
           } else {
                throw std::runtime_error("constituent is not a PackedCandidate or PackedGenCandidate");
            }
        } // end for part

        if(verbose_){
            printf("rawPt = %f\n", rawpt);
            printf("pt = %f\n", pt);
            printf("sumpt = %f\n", ans.sumpt);
            printf("jec = %f\n", jecfactor);
        }

        if(rescaleMiniAODtruncation_){
            double factor = rawpt / ans.sumpt;
            for(auto& part : ans.particles){
                part.pt *= factor;
            }
            ans.sumpt *= factor;
        }
        
        if(ans.nPart >= 2){
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

typedef SimonJetProducerT<pat::Jet> PatSimonJetProducer;
typedef SimonJetProducerT<reco::GenJet> GenSimonJetProducer;

DEFINE_FWK_MODULE(PatSimonJetProducer);
DEFINE_FWK_MODULE(GenSimonJetProducer);
