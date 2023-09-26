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

#include "SRothman/SimonTools/src/jets.h"
#include "SRothman/SimonTools/src/util.h"

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
    double minPartPt_;
    unsigned int maxNumPart_, minNumPart_;

    int verbose_;

    double minPt_;
    double maxEta_;

    double maxMuFrac_;
    double maxChEmFrac_;

    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<T>> srcToken_;

    edm::InputTag evtSelSrc_;
    edm::EDGetTokenT<bool> evtSelToken_;
    bool doEvtSel_;

    bool applyJEC_;
};

template <typename T>
SimonJetProducerT<T>::SimonJetProducerT(const edm::ParameterSet& conf)
        : minPartPt_(conf.getParameter<double>("minPartPt")),
          maxNumPart_(conf.getParameter<unsigned>("maxNumPart")),
          minNumPart_(conf.getParameter<unsigned>("minNumPart")),
          verbose_(conf.getParameter<int>("verbose")),
          minPt_(conf.getParameter<double>("minPt")),
          maxEta_(conf.getParameter<double>("maxEta")),
          maxMuFrac_(conf.getParameter<double>("maxMuFrac")),
          maxChEmFrac_(conf.getParameter<double>("maxChEmFrac")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<T>>(src_)),
          evtSelSrc_(conf.getParameter<edm::InputTag>("eventSelection")),
          evtSelToken_(consumes<bool>(evtSelSrc_)),
          doEvtSel_(conf.getParameter<bool>("doEventSelection")),
          applyJEC_(conf.getParameter<bool>("applyJEC")){
    produces<std::vector<jet>>();
}

template <typename T>
void SimonJetProducerT<T>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<double>("minPartPt");
  desc.add<unsigned>("maxNumPart");
  desc.add<unsigned>("minNumPart");

  desc.add<double>("minPt");
  desc.add<double>("maxEta");
  desc.add<double>("maxMuFrac");
  desc.add<double>("maxChEmFrac");

  desc.add<edm::InputTag>("eventSelection");
  desc.add<bool>("doEventSelection");

  desc.add<edm::InputTag>("src");

  desc.add<int>("verbose");

  desc.add<bool>("applyJEC");

  descriptions.addWithDefaultLabel(desc);
}

template <typename T>
void SimonJetProducerT<T>::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of SimonJetProducerT<T>::produce()\n");
    }
  edm::Handle<edm::View<T>> jets;
  evt.getByToken(srcToken_, jets);

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

  for(unsigned iJet=0; iJet < jets->size(); ++iJet){

    const auto& j = jets->at(iJet);

    if(j.pt() < minPt_ || std::fabs(j.eta()) > maxEta_){
        continue;
    } 

    double chEmFrac=0;
    double muFrac=0;
    if constexpr(std::is_same<T, pat::Jet>::value){
        chEmFrac = j.chargedEmEnergyFraction();
        muFrac = j.muonEnergyFraction();
    } else if constexpr(std::is_same<T, reco::Jet>::value){
        if(maxChEmFrac_>0 || maxMuFrac_>0){
            throw std::logic_error("cutting on lepton energy fractions not supported for reco jets");
        }
        chEmFrac = 0;
        muFrac = 0;
    } else if constexpr(std::is_same<T, reco::GenJet>::value){
        chEmFrac = j.chargedEmEnergy() / j.energy();
        muFrac = j.muonEnergy() / j.energy();
    }

    if(chEmFrac > maxChEmFrac_ || muFrac > maxMuFrac_){
        continue;
    }

    const std::vector<reco::Jet::Constituent>& constituents = j.getJetConstituents();

    if(constituents.size() < minNumPart_){
        continue;
    }

    if(verbose_){
        printf("jet %d passed jet selection\n", iJet);
    }

    double pt = j.pt();
    double eta = j.eta();
    double phi = j.phi();

    jet ans;
    ans.pt = pt;
    ans.eta = eta;
    ans.phi = phi;
    ans.nPart = 0;
    ans.iJet = iJet;
    ans.sumpt = 0;
    ans.particles.clear();

    if (verbose_){
      std::cout << "\tjet: (" << pt << ", " << eta << ", " << phi << ")" << std::endl;
    }

    size_t nPart=std::min<size_t>(maxNumPart_, constituents.size());
    if(verbose_>1){
        printf("constituents.size() = %lu\n", constituents.size());
        printf("maxNumPart_ = %u\n", maxNumPart_);
        printf("nPart = %lu\n", nPart);
    }
    for(unsigned iPart=0; iPart < nPart; ++iPart){
        const auto& part = constituents[iPart];
        if(verbose_>1){
            printf("part %d: (%f, %f, %f)\n", iPart, part->pt(), part->eta(), part->phi());
        }
        ans.sumpt += part->pt();
        if(part->pt() >= minPartPt_){
            particle next(part->pt(), part->eta(), part->phi(), 
                          0.0, 0.0, 0.0,
                          std::abs(part->pdgId()), part->charge());
            ans.particles.push_back(std::move(next));
            ++ans.nPart;
        }
    }

    if(applyJEC_){
        double rawPt=0;
        for(const auto& part : constituents){
            rawPt += part->pt();
        }
        double jec = pt / rawPt;

        if(verbose_){
            printf("rawPt = %f\n", rawPt);
            printf("pt = %f\n", pt);
            printf("sumpt = %f\n", ans.sumpt);
            printf("jec = %f\n", jec);
        }

        for(auto& part : ans.particles){
            part.pt *= jec;
        }
        ans.sumpt *= jec;
    }
    result->push_back(std::move(ans));
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
typedef SimonJetProducerT<reco::PFJet> RecoSimonJetProducer;
typedef SimonJetProducerT<reco::GenJet> GenSimonJetProducer;

DEFINE_FWK_MODULE(PatSimonJetProducer);
DEFINE_FWK_MODULE(RecoSimonJetProducer);
DEFINE_FWK_MODULE(GenSimonJetProducer);
