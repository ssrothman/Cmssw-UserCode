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

bool hasNearby(const jet& source, const reco::Candidate& cand){
    for(const auto& part : source.particles){
        double dist = dR2(part.eta, part.phi,
                        cand.eta(), cand.phi());
        if(dist < 0.2*0.2){
            return true;
        }
    }
    return false;
}

void makeNearbyJet(jet& result, const jet& source, 
                 const edm::Handle<edm::View<reco::Candidate>>& candidates){
    result.nPart = 0;
    result.sumpt = 0.0;
    result.particles.clear();

    for(const auto& cand : *candidates){
        if(cand.pt() <= 1e-3){
            continue;
        }
        if(std::abs(cand.pdgId()) < 10){
            printf("warning: particle with pdgId=%d???? (pT = %0.4f) Skipping\n", cand.pdgId(), cand.pt());
            continue;
        }
        if(hasNearby(source, cand)){
            particle nextpart(cand.pt(), cand.eta(), cand.phi(),
                            0.0, 0.0, 0.0,
                            std::abs(cand.pdgId()), cand.charge());
            result.particles.push_back(std::move(nextpart));
            result.nPart++;
            result.sumpt += cand.pt();
        }
    } 
    //TODO: sort?
}

class GenMatchProducer : public edm::stream::EDProducer<> {
public:
    explicit GenMatchProducer(const edm::ParameterSet&);
    ~GenMatchProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    int verbose_;

    double jetMatchingDR2_;
        
    double clipval_;

    enum spatialLoss loss_;
    std::vector<double> PUpt0s_, PUexps_, PUpenalties_;

    std::string uncertainty_;

    std::vector<std::string> softflavorfilters_;
    std::vector<std::string> hardflavorfilters_;
    std::vector<double> filterthresholds_;

    std::vector<std::string> chargefilters_;

    std::vector<std::string> prefitters_;
    std::string refiner_;
    std::string dropGenFilter_;
    std::string dropRecoFilter_;

    bool recoverLostTracks_;
    std::vector<double> HADCHrecoverThresholds_;
    std::vector<double> ELErecoverThresholds_;

    bool recoverLostHAD0_;
    std::vector<double> HAD0recoverThresholds_;

    std::vector<double> EMstochastic_, EMconstant_;
    std::vector<double> ECALgranularityEta_, ECALgranularityPhi_;
    std::vector<double> ECALEtaBoundaries_;

    std::vector<double> HADstochastic_, HADconstant_;
    std::vector<double> HCALgranularityEta_, HCALgranularityPhi_;
    std::vector<double> HCALEtaBoundaries_;

    std::vector<double> CHlinear_, CHconstant_;
    std::vector<double> CHMSeta_, CHMSphi_;
    std::vector<double> CHangularEta_, CHangularPhi_;
    std::vector<double> trkEtaBoundaries_;

    std::vector<double> EM0thresholds_, HAD0thresholds_;
    std::vector<double> HADCHthresholds_, ELEthresholds_, MUthresholds_;

    std::vector<double> EM0dRcuts_, HAD0dRcuts_;
    std::vector<double> HADCHdRcuts_, ELEdRcuts_, MUdRcuts_;

    unsigned maxReFit_;

    edm::InputTag recoTag_;
    edm::EDGetTokenT<edm::View<jet>> recoToken_;
    edm::InputTag genTag_;
    edm::EDGetTokenT<edm::View<jet>> genToken_;
};

GenMatchProducer::GenMatchProducer(const edm::ParameterSet& conf)
                : verbose_(conf.getParameter<int>("verbose")),
                jetMatchingDR2_(square(conf.getParameter<double>("jetMatchingDR"))),

                clipval_(conf.getParameter<double>("clipval")),

                loss_(static_cast<enum spatialLoss>(conf.getParameter<int>("spatialLoss"))),
                PUpt0s_(conf.getParameter<std::vector<double>>("PUpt0s")),
                PUexps_(conf.getParameter<std::vector<double>>("PUexps")),
                PUpenalties_(conf.getParameter<std::vector<double>>("PUpenalties")),

                uncertainty_(conf.getParameter<std::string>("uncertainty")),

                softflavorfilters_(conf.getParameter<std::vector<std::string>>("softflavorfilters")),
                hardflavorfilters_(conf.getParameter<std::vector<std::string>>("hardflavorfilters")),
                filterthresholds_(conf.getParameter<std::vector<double>>("filterthresholds")),
                chargefilters_(conf.getParameter<std::vector<std::string>>("chargefilters")),

                prefitters_(conf.getParameter<std::vector<std::string>>("prefitters")),
                refiner_(conf.getParameter<std::string>("refiner")),
                dropGenFilter_(conf.getParameter<std::string>("dropGenFilter")),
                dropRecoFilter_(conf.getParameter<std::string>("dropRecoFilter")),

                recoverLostTracks_(conf.getParameter<bool>("recoverLostTracks")),
                HADCHrecoverThresholds_(conf.getParameter<std::vector<double>>("HADCHrecoverThresholds")),
                ELErecoverThresholds_(conf.getParameter<std::vector<double>>("ELErecoverThresholds")),
                recoverLostHAD0_(conf.getParameter<bool>("recoverLostHAD0")),
                HAD0recoverThresholds_(conf.getParameter<std::vector<double>>("HAD0recoverThresholds")),

                EMstochastic_(conf.getParameter<std::vector<double>>("EMstochastic")),
                EMconstant_(conf.getParameter<std::vector<double>>("EMconstant")),
                ECALgranularityEta_(conf.getParameter<std::vector<double>>("ECALgranularityEta")),
                ECALgranularityPhi_(conf.getParameter<std::vector<double>>("ECALgranularityPhi")),
                ECALEtaBoundaries_(conf.getParameter<std::vector<double>>("ECALEtaBoundaries")),

                HADstochastic_(conf.getParameter<std::vector<double>>("HADstochastic")),
                HADconstant_(conf.getParameter<std::vector<double>>("HADconstant")),
                HCALgranularityEta_(conf.getParameter<std::vector<double>>("HCALgranularityEta")),
                HCALgranularityPhi_(conf.getParameter<std::vector<double>>("HCALgranularityPhi")),
                HCALEtaBoundaries_(conf.getParameter<std::vector<double>>("HCALEtaBoundaries")),

                CHlinear_(conf.getParameter<std::vector<double>>("CHlinear")),
                CHconstant_(conf.getParameter<std::vector<double>>("CHconstant")),
                CHMSeta_(conf.getParameter<std::vector<double>>("CHMSeta")),
                CHMSphi_(conf.getParameter<std::vector<double>>("CHMSphi")),
                CHangularEta_(conf.getParameter<std::vector<double>>("CHangularEta")),
                CHangularPhi_(conf.getParameter<std::vector<double>>("CHangularPhi")),
                trkEtaBoundaries_(conf.getParameter<std::vector<double>>("trkEtaBoundaries")),

                EM0thresholds_(conf.getParameter<std::vector<double>>("EM0thresholds")),
                HAD0thresholds_(conf.getParameter<std::vector<double>>("HAD0thresholds")),
                HADCHthresholds_(conf.getParameter<std::vector<double>>("HADCHthresholds")),
                ELEthresholds_(conf.getParameter<std::vector<double>>("ELEthresholds")),
                MUthresholds_(conf.getParameter<std::vector<double>>("MUthresholds")),

                EM0dRcuts_(conf.getParameter<std::vector<double>>("EM0dRcuts")),
                HAD0dRcuts_(conf.getParameter<std::vector<double>>("HAD0dRcuts")),
                HADCHdRcuts_(conf.getParameter<std::vector<double>>("HADCHdRcuts")),
                ELEdRcuts_(conf.getParameter<std::vector<double>>("ELEdRcuts")),
                MUdRcuts_(conf.getParameter<std::vector<double>>("MUdRcuts")),

                maxReFit_(conf.getParameter<unsigned>("maxReFit")),

                recoTag_(conf.getParameter<edm::InputTag>("reco")),
                recoToken_(consumes<edm::View<jet>>(recoTag_)),
                genTag_(conf.getParameter<edm::InputTag>("gen")),
                genToken_(consumes<edm::View<jet>>(genTag_)){
    produces<std::vector<jetmatch>>();
}

void GenMatchProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    desc.add<double>("jetMatchingDR");

    desc.add<double>("clipval");

    desc.add<int>("spatialLoss");
    desc.add<std::vector<double>>("PUpt0s");
    desc.add<std::vector<double>>("PUexps");
    desc.add<std::vector<double>>("PUpenalties");

    desc.add<std::string>("uncertainty");

    desc.add<std::vector<std::string>>("softflavorfilters");
    desc.add<std::vector<std::string>>("hardflavorfilters");
    desc.add<std::vector<double>>("filterthresholds");

    desc.add<std::vector<std::string>>("chargefilters");

    desc.add<std::vector<std::string>>("prefitters");
    desc.add<std::string>("refiner");
    desc.add<std::string>("dropGenFilter");
    desc.add<std::string>("dropRecoFilter");

    desc.add<bool>("recoverLostTracks");
    desc.add<std::vector<double>>("HADCHrecoverThresholds");
    desc.add<std::vector<double>>("ELErecoverThresholds");

    desc.add<bool>("recoverLostHAD0");
    desc.add<std::vector<double>>("HAD0recoverThresholds");

    desc.add<std::vector<double>>("EMstochastic");
    desc.add<std::vector<double>>("EMconstant");
    desc.add<std::vector<double>>("ECALgranularityEta");
    desc.add<std::vector<double>>("ECALgranularityPhi");
    desc.add<std::vector<double>>("ECALEtaBoundaries");

    desc.add<std::vector<double>>("HADstochastic");
    desc.add<std::vector<double>>("HADconstant");
    desc.add<std::vector<double>>("HCALgranularityEta");
    desc.add<std::vector<double>>("HCALgranularityPhi");
    desc.add<std::vector<double>>("HCALEtaBoundaries");

    desc.add<std::vector<double>>("CHlinear");
    desc.add<std::vector<double>>("CHconstant");
    desc.add<std::vector<double>>("CHMSeta");
    desc.add<std::vector<double>>("CHMSphi");
    desc.add<std::vector<double>>("CHangularEta");
    desc.add<std::vector<double>>("CHangularPhi");
    desc.add<std::vector<double>>("trkEtaBoundaries");

    desc.add<std::vector<double>>("EM0thresholds");
    desc.add<std::vector<double>>("HAD0thresholds");
    desc.add<std::vector<double>>("HADCHthresholds");
    desc.add<std::vector<double>>("ELEthresholds");
    desc.add<std::vector<double>>("MUthresholds");

    desc.add<std::vector<double>>("EM0dRcuts");
    desc.add<std::vector<double>>("HAD0dRcuts");
    desc.add<std::vector<double>>("HADCHdRcuts");
    desc.add<std::vector<double>>("ELEdRcuts");
    desc.add<std::vector<double>>("MUdRcuts");

    desc.add<unsigned>("maxReFit");

    desc.add<edm::InputTag>("reco");
    desc.add<edm::InputTag>("gen");

    desc.add<int>("verbose");
    descriptions.addWithDefaultLabel(desc);
}

void printParticle(const particle& part){
    printf("pt: %f, eta: %f, phi: %f, pdgId: %d, charge: %d\n",
             part.pt, part.eta, part.phi, part.pdgid, part.charge);
}
void printJet(const jet& jet){
    for(const auto& part : jet.particles){
        printParticle(part);
    }
}

void GenMatchProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("Top of GenMatchProducer::produce()\n");
    }
    edm::Handle<edm::View<jet>> reco;
    evt.getByToken(recoToken_, reco);

    edm::Handle<edm::View<jet>> gen;
    evt.getByToken(genToken_, gen);

    auto result = std::make_unique<std::vector<jetmatch>>();
    auto resultBigGen = std::make_unique<std::vector<jetmatch>>();
    auto resultBigReco = std::make_unique<std::vector<jetmatch>>();

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
            jetmatch next;
            next.iReco = iReco;
            next.iGen = matchedgen;

            const jet& jgen = gen->at(matchedgen);

            if(verbose_){
                printf("fit is between %lu and %lu particles\n", 
                        jreco.particles.size(), jgen.particles.size());

                printf("\nRECO JET\n");
                printJet(jreco);
                printf("\nGEN JET\n");
                printJet(jgen);
            }

            if(verbose_)
                printf("doing fit\n");

            std::unique_ptr<matcher> thismatch = std::make_unique<matcher>(
                jreco, jgen, 
                clipval_,
                loss_, PUpt0s_,
                PUexps_, PUpenalties_,
                uncertainty_,
                softflavorfilters_, 
                hardflavorfilters_,
                filterthresholds_,
                chargefilters_,
                prefitters_,
                refiner_,
                dropGenFilter_, dropRecoFilter_,
                recoverLostTracks_,
                HADCHrecoverThresholds_,
                ELErecoverThresholds_,
                recoverLostHAD0_,
                HAD0recoverThresholds_,
                EMstochastic_, EMconstant_,
                ECALgranularityEta_, ECALgranularityPhi_,
                ECALEtaBoundaries_,
                HADstochastic_, HADconstant_,
                HCALgranularityEta_, HCALgranularityPhi_,
                HCALEtaBoundaries_,
                CHlinear_, CHconstant_, 
                CHMSeta_, CHMSphi_,
                CHangularEta_, CHangularPhi_,
                trkEtaBoundaries_, 
                EM0thresholds_, HAD0thresholds_, 
                HADCHthresholds_, ELEthresholds_, MUthresholds_,
                EM0dRcuts_, HAD0dRcuts_, 
                HADCHdRcuts_, ELEdRcuts_, MUdRcuts_,

                maxReFit_, verbose_);
            thismatch->minimize();

            next.ptrans = thismatch->ptrans();
            next.rawmat = thismatch->rawmat();

            if(verbose_){
                printf("\nMatching complete:\n");
                std::cout << next.rawmat;
            }

            result->push_back(std::move(next));
            if(verbose_){
                printf("did fit\n");
            }
        }//end if matched
    }//end for each reco

    evt.put(std::move(result));
}  // end produce()

DEFINE_FWK_MODULE(GenMatchProducer);
