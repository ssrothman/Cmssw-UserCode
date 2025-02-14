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

#include "SRothman/DataFormats/interface/matching.h"
#include "SRothman/DataFormats/interface/jets.h"

#include <Eigen/Dense>

class TrackMatchTableProducer : public edm::stream::EDProducer<> {
public:
    explicit TrackMatchTableProducer(const edm::ParameterSet&);
    ~TrackMatchTableProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

    static constexpr int MATCH_MUON = 1;
    static constexpr int MATCH_ELE = 2;
    static constexpr int MATCH_PHO = 4;
    static constexpr int MATCH_HADCH = 8;
    static constexpr int MATCH_PI0 = 16;
    static constexpr int MATCH_HAD0 = 32;
private:
    std::string genname_, reconame_;

    edm::EDGetTokenT<std::vector<simon::jet>> recoJetsToken_;
    edm::EDGetTokenT<std::vector<simon::jet>> genJetsToken_;
    edm::EDGetTokenT<std::vector<matching::jetmatch>> matchToken_;
};

TrackMatchTableProducer::TrackMatchTableProducer(const edm::ParameterSet& config) :
        genname_(config.getParameter<std::string>("genname")),
        reconame_(config.getParameter<std::string>("reconame")),
        recoJetsToken_(consumes<std::vector<simon::jet>>(config.getParameter<edm::InputTag>("recoJets"))),
        genJetsToken_(consumes<std::vector<simon::jet>>(config.getParameter<edm::InputTag>("genJets"))),
        matchToken_(consumes<std::vector<matching::jetmatch>>(config.getParameter<edm::InputTag>("matches")))
{
    produces<nanoaod::FlatTable>("recojet");
    produces<nanoaod::FlatTable>("genjet");
}

void TrackMatchTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<std::string>("genname");
    desc.add<std::string>("reconame");
    desc.add<edm::InputTag>("recoJets");
    desc.add<edm::InputTag>("genJets");
    desc.add<edm::InputTag>("matches");
    descriptions.addWithDefaultLabel(desc);
}

void TrackMatchTableProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
    edm::Handle<std::vector<simon::jet>> recojets;
    event.getByToken(recoJetsToken_, recojets);

    edm::Handle<std::vector<simon::jet>> genjets;
    event.getByToken(genJetsToken_, genjets);

    edm::Handle<std::vector<matching::jetmatch>> matches;
    event.getByToken(matchToken_, matches);

    std::vector<float> matchPt;
    std::vector<float> matchEta;
    std::vector<float> matchPhi;
    std::vector<int> matchCharge;
    std::vector<int> nMatches;
    std::vector<int> matchTypes;

    for(unsigned iReco=0; iReco<recojets->size(); ++iReco){
        const matching::jetmatch * thematch=nullptr;

        for(const auto& match : *matches){
            if(match.iReco == iReco){
                thematch = &match;
                break;
            }
        }

        const auto& recojet = recojets->at(iReco);

        if(thematch){
            const auto& genjet = genjets->at(thematch->iGen);

            std::vector<float> matchPt_(recojet.nPart, 0);
            std::vector<float> matchEta_(recojet.nPart, 0);
            std::vector<float> matchPhi_(recojet.nPart, 0);
            std::vector<int> matchCharge_(recojet.nPart, 0);
            std::vector<int> nMatches_(recojet.nPart, 0);
            std::vector<int> matchTypes_(recojet.nPart, 0);

            for(unsigned iRecoPart=0; iRecoPart < recojet.nPart; ++iRecoPart){
                for(unsigned iGenPart=0; iGenPart < genjet.nPart; ++iGenPart){
                    const auto& genPart = genjet.particles.at(iGenPart);
                    double tmatval = thematch->tmat(iRecoPart, iGenPart);
                    if(tmatval > 0){
                        matchPt_[iRecoPart] += genPart.pt * tmatval;
                        matchEta_[iRecoPart] += genPart.pt * genPart.eta * tmatval;
                        matchPhi_[iRecoPart] += genPart.pt * genPart.phi * tmatval;
                        matchCharge_[iRecoPart] += genPart.charge;
                        ++nMatches_[iRecoPart];

                        if(genPart.pdgid == 13){
                            matchTypes_[iRecoPart] |= MATCH_MUON;
                        } else if(genPart.pdgid == 11){
                            matchTypes_[iRecoPart] |= MATCH_ELE;
                        } else if(genPart.pdgid == 22){
                            matchTypes_[iRecoPart] |= MATCH_PHO;
                        } else if(genPart.pdgid == 111){
                            matchTypes_[iRecoPart] |= MATCH_PI0;
                        } else if(genPart.charge != 0){
                            matchTypes_[iRecoPart] |= MATCH_HADCH;
                        } else{
                            matchTypes_[iRecoPart] |= MATCH_HAD0;
                        }
                    }//if gen, reco match
                }//end for gen loop
                if (matchPt_[iRecoPart] > 0){
                    matchEta_[iRecoPart] /= matchPt_[iRecoPart];
                    matchPhi_[iRecoPart] /= matchPt_[iRecoPart];
                }
            }//end for reco loop
            
            matchPt.insert(matchPt.end(), matchPt_.begin(), matchPt_.end());
            matchEta.insert(matchEta.end(), matchEta_.begin(), matchEta_.end());
            matchPhi.insert(matchPhi.end(), matchPhi_.begin(), matchPhi_.end());
            matchCharge.insert(matchCharge.end(), matchCharge_.begin(), matchCharge_.end());
            nMatches.insert(nMatches.end(), nMatches_.begin(), nMatches_.end());
            matchTypes.insert(matchTypes.end(), matchTypes_.begin(), matchTypes_.end());

        } else{ //no match, just push a bunch of zeroes
            for(unsigned i=0; i<recojet.nPart; ++i){
                matchPt.push_back(0);
                matchEta.push_back(0);
                matchPhi.push_back(0);
                matchCharge.push_back(0);
                nMatches.push_back(-1); //negative 1 = no matched jet
                                        //whereas  0 = no matched particle
                matchTypes.push_back(0);
            }
        }
    }

    auto result = std::make_unique<nanoaod::FlatTable>(matchPt.size(), reconame_, false, true); //extension table
    result->addColumn<float>("matchPt", matchPt, "matched pT", nanoaod::FlatTable::FloatColumn);
    result->addColumn<float>("matchEta", matchEta, "matched eta", nanoaod::FlatTable::FloatColumn);
    result->addColumn<float>("matchPhi", matchPhi, "matched phi", nanoaod::FlatTable::FloatColumn);
    result->addColumn<int>("matchCharge", matchCharge, "matched charge", nanoaod::FlatTable::IntColumn);
    result->addColumn<int>("nMatches", nMatches, "number of matched particles", nanoaod::FlatTable::IntColumn);
    result->addColumn<int>("matchTypes", matchTypes, "matched particle types", nanoaod::FlatTable::IntColumn);
    event.put(std::move(result), "recojet");

    std::vector<int> gen_nMatches;
    std::vector<int> gen_matchTypes;
    for(unsigned iGen=0; iGen<genjets->size(); ++iGen){
        const matching::jetmatch * thematch=nullptr;
        for(const auto& match : *matches){
            if(match.iGen == iGen){
                thematch = &match;
                break;
            }
        }

        const auto& genjet = genjets->at(iGen);

        if(thematch){
            const auto& recojet = recojets->at(thematch->iReco);
            
            std::vector<int> gen_nMatches_(genjet.nPart, 0);
            std::vector<int> gen_matchTypes_(genjet.nPart, 0);

            for(unsigned iRecoPart=0; iRecoPart < recojet.nPart; ++iRecoPart){
                const auto& recoPart = recojet.particles.at(iRecoPart);
                for(unsigned iGenPart=0; iGenPart < genjet.nPart; ++iGenPart){
                    double tmatval = thematch->tmat(iRecoPart, iGenPart);
                    if(tmatval > 0){
                        ++gen_nMatches_[iGenPart];
                        if(recoPart.pdgid == 13){
                            gen_matchTypes_[iGenPart] |= MATCH_MUON;
                        } else if(recoPart.pdgid == 11){
                            gen_matchTypes_[iGenPart] |= MATCH_ELE;
                        } else if(recoPart.pdgid == 22){
                            gen_matchTypes_[iGenPart] |= MATCH_PHO;
                        } else if(recoPart.pdgid == 111){
                            gen_matchTypes_[iGenPart] |= MATCH_PI0;
                        } else if(recoPart.charge != 0){
                            gen_matchTypes_[iGenPart] |= MATCH_HADCH;
                        } else{
                            gen_matchTypes_[iGenPart] |= MATCH_HAD0;
                        }
                    }
                }
            }

            gen_nMatches.insert(gen_nMatches.end(), gen_nMatches_.begin(), gen_nMatches_.end());
            gen_matchTypes.insert(gen_matchTypes.end(), gen_matchTypes_.begin(), gen_matchTypes_.end());
        } else {
            for(unsigned i=0; i<genjet.nPart; ++i){
                gen_nMatches.push_back(-1);
                gen_matchTypes.push_back(0);
            }
        }
    } 

    auto genresult = std::make_unique<nanoaod::FlatTable>(gen_nMatches.size(), genname_, false, true); //extension table
    genresult->addColumn<int>("nMatches", gen_nMatches, "number of matched particles", nanoaod::FlatTable::IntColumn);
    genresult->addColumn<int>("matchTypes", gen_matchTypes, "matched particle types", nanoaod::FlatTable::IntColumn);
    event.put(std::move(genresult), "genjet");
}

DEFINE_FWK_MODULE(TrackMatchTableProducer);
