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

#include "SRothman/DataFormats/interface/jets.h"
#include "SRothman/DataFormats/interface/matching.h"
#include "SRothman/SimonTools/src/isID.h"

#include <iostream>
#include <memory>
#include <vector>

#include <Eigen/Dense>

class CHSSumTableProducer : public edm::stream::EDProducer<> {
public:
    explicit CHSSumTableProducer(const edm::ParameterSet&);
    ~CHSSumTableProducer() override {}
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;

private:
    std::string name_;
    
    edm::InputTag src_;
    edm::EDGetTokenT<edm::View<simon::jet>> srcToken_;

    edm::InputTag CHSsrc_;
    edm::EDGetTokenT<edm::View<pat::Jet>> CHSsrcToken_;

    int verbose_;
};

CHSSumTableProducer::CHSSumTableProducer(const edm::ParameterSet& conf)
        : name_(conf.getParameter<std::string>("name")),
          src_(conf.getParameter<edm::InputTag>("src")),
          srcToken_(consumes<edm::View<simon::jet>>(src_)),
          CHSsrc_(conf.getParameter<edm::InputTag>("CHSsrc")),
          CHSsrcToken_(consumes<edm::View<pat::Jet>>(CHSsrc_)),
          verbose_(conf.getParameter<int>("verbose")){

    produces<nanoaod::FlatTable>(name_);
}

void CHSSumTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<std::string>("name");
    desc.add<edm::InputTag>("src");
    desc.add<edm::InputTag>("CHSsrc");
    desc.add<int>("verbose");
    descriptions.add("CHSSumTableProducer", desc);
}

void CHSSumTableProducer::produce(edm::Event& evt, const edm::EventSetup& setup) {
    if(verbose_){
        printf("top of CHSSumTableProducer::produce()\n");
    }

    edm::Handle<edm::View<simon::jet>> src;
    evt.getByToken(srcToken_, src);

    edm::Handle<edm::View<pat::Jet>> CHSsrc;
    evt.getByToken(CHSsrcToken_, CHSsrc);

    std::vector<float> CHSpt, CHSeta, CHSphi;
    std::vector<int> CHSparton, CHShadron;
    std::vector<float> btagDeepB, btagDeepFlavB, btagCSVV2;
    std::vector<float> btagDeepCvL, btagDeepCvB;
    std::vector<float> btagDeepFlavCvL, btagDeepFlavCvB;
    std::vector<float> btagDeepFlavQG;
    std::vector<float> qgl;

    for (const auto& simonjet : *src) {
        float CHSpt_ = 0;
        float CHSeta_ = 0;
        float CHSphi_ = 0;
        int CHSparton_ = -1;
        int CHShadron_ = -1;
        float btagDeepB_ = -1;
        float btagDeepFlavB_ = -1;
        float btagCSVV2_ = -1;
        float btagDeepCvL_ = -1;
        float btagDeepCvB_ = -1;
        float btagDeepFlavCvL_ = -1;
        float btagDeepFlavCvB_ = -1;
        float btagDeepFlavQG_ = -1;
        float qgl_ = -1;

        for(const auto iCHS : simonjet.iCHS){
            if (iCHS == 99999999) continue;
            const auto& jCHS = CHSsrc->at(iCHS);
            CHSpt_ += jCHS.pt();
            CHSeta_ += jCHS.eta() * jCHS.pt();
            CHSphi_ += jCHS.phi() * jCHS.pt();
            CHSparton_ = std::max({CHSparton_, jCHS.partonFlavour()});
            CHShadron_ = std::max({CHShadron_, jCHS.hadronFlavour()});
    
            float tmp;
            tmp = jCHS.bDiscriminator("pfDeepCSVJetTags:probb") + jCHS.bDiscriminator("pfDeepCSVJetTags:probbb");
            btagDeepB_ = std::max({btagDeepB_, tmp});
            
            tmp = jCHS.bDiscriminator("pfDeepFlavourJetTags:probb") + jCHS.bDiscriminator("pfDeepFlavourJetTags:probbb") + jCHS.bDiscriminator("pfDeepFlavourJetTags:problepb");
            btagDeepFlavB_ = std::max({btagDeepFlavB_, tmp});

            tmp = jCHS.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
            btagCSVV2_ = std::max({btagCSVV2_, tmp});

            tmp = jCHS.bDiscriminator("pfDeepCSVJetTags:probc");
            float denom = jCHS.bDiscriminator("pfDeepCSVJetTags:probc") + jCHS.bDiscriminator("pfDeepCSVJetTags:probudsg");
            if (denom > 0) tmp = tmp / denom;
            else tmp = -1;
            btagDeepCvL_ = std::max({btagDeepCvL_, tmp});

            tmp = jCHS.bDiscriminator("pfDeepCSVJetTags:probc");
            denom = jCHS.bDiscriminator("pfDeepCSVJetTags:probc") + jCHS.bDiscriminator("pfDeepCSVJetTags:probb") + jCHS.bDiscriminator("pfDeepCSVJetTags:probbb");
            if (denom > 0) tmp = tmp / denom;
            else tmp = -1;
            btagDeepCvB_ = std::max({btagDeepCvB_, tmp});

            tmp = jCHS.bDiscriminator("pfDeepFlavourJetTags:probc");
            denom = jCHS.bDiscriminator("pfDeepFlavourJetTags:probc") + jCHS.bDiscriminator("pfDeepFlavourJetTags:probuds") + jCHS.bDiscriminator("pfDeepFlavourJetTags:probg");
            if (denom > 0) tmp = tmp / denom;
            else tmp = -1;
            btagDeepFlavCvL_ = std::max({btagDeepFlavCvL_, tmp});

            tmp = jCHS.bDiscriminator("pfDeepFlavourJetTags:probc");
            denom = jCHS.bDiscriminator("pfDeepFlavourJetTags:probc") + jCHS.bDiscriminator("pfDeepFlavourJetTags:probb") + jCHS.bDiscriminator("pfDeepFlavourJetTags:probbb") + jCHS.bDiscriminator("pfDeepFlavourJetTags:problepb");
            if (denom > 0) tmp = tmp / denom;
            else tmp = -1;
            btagDeepFlavCvB_ = std::max({btagDeepFlavCvB_, tmp});

            tmp = jCHS.bDiscriminator("pfDeepFlavourJetTags:probg");
            denom = jCHS.bDiscriminator("pfDeepFlavourJetTags:probg") + jCHS.bDiscriminator("pfDeepFlavourJetTags:probuds");
            if (denom > 0) tmp = tmp / denom;
            else tmp = -1;
            btagDeepFlavQG_ = std::max({btagDeepFlavQG_, tmp});

            qgl_ = std::max({qgl_, jCHS.userFloat("QGTagger:qgLikelihood")});
        }

        if(CHSpt_ > 0){
            CHSeta_ = CHSeta_ / CHSpt_;
            CHSphi_ = CHSphi_ / CHSpt_;
        }

        CHSpt.push_back(CHSpt_);
        CHSeta.push_back(CHSeta_);
        CHSphi.push_back(CHSphi_);
        CHSparton.push_back(CHSparton_);
        CHShadron.push_back(CHShadron_);
        btagDeepB.push_back(btagDeepB_);
        btagDeepFlavB.push_back(btagDeepFlavB_);
        btagCSVV2.push_back(btagCSVV2_);
        btagDeepCvL.push_back(btagDeepCvL_);
        btagDeepCvB.push_back(btagDeepCvB_);
        btagDeepFlavCvL.push_back(btagDeepFlavCvL_);
        btagDeepFlavCvB.push_back(btagDeepFlavCvB_);
        btagDeepFlavQG.push_back(btagDeepFlavQG_);
        qgl.push_back(qgl_);
    }

    auto out = std::make_unique<nanoaod::FlatTable>(CHSpt.size(), name_, false, true);
    out->addColumn<float>("CHSpt", CHSpt, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<float>("CHSeta", CHSeta, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<float>("CHSphi", CHSphi, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<int>("CHSparton", CHSparton, "I", nanoaod::FlatTable::IntColumn);
    out->addColumn<int>("CHShadron", CHShadron, "I", nanoaod::FlatTable::IntColumn);
    out->addColumn<float>("CHSbtagDeepB", btagDeepB, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<float>("CHSbtagDeepFlavB", btagDeepFlavB, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<float>("CHSbtagCSVV2", btagCSVV2, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<float>("CHSbtagDeepCvL", btagDeepCvL, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<float>("CHSbtagDeepCvB", btagDeepCvB, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<float>("CHSbtagDeepFlavCvL", btagDeepFlavCvL, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<float>("CHSbtagDeepFlavCvB", btagDeepFlavCvB, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<float>("CHSbtagDeepFlavQG", btagDeepFlavQG, "F", nanoaod::FlatTable::FloatColumn);
    out->addColumn<float>("CHSqgl", qgl, "F", nanoaod::FlatTable::FloatColumn);
    evt.put(std::move(out), name_);
}

DEFINE_FWK_MODULE(CHSSumTableProducer);
