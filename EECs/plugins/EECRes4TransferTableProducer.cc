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

#include "SRothman/DataFormats/interface/EEC.h"

class EECRes4TransferTableProducer : public edm::stream::EDProducer<> {
public:
    explicit EECRes4TransferTableProducer(const edm::ParameterSet&);
    ~EECRes4TransferTableProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    std::string name_;

    edm::EDGetTokenT<std::vector<EEC::CMSSW_Res4TransferResult>> EECTransferToken_;
};

EECRes4TransferTableProducer::EECRes4TransferTableProducer(const edm::ParameterSet& conf) :
        name_(conf.getParameter<std::string>("name")),
        EECTransferToken_(consumes<std::vector<EEC::CMSSW_Res4TransferResult>>(conf.getParameter<edm::InputTag>("EECTransfer"))) {

    produces<nanoaod::FlatTable>(name_+"TRANSFER_dipole_transfered");
    produces<nanoaod::FlatTable>(name_+"TRANSFER_tee_transfered");
    produces<nanoaod::FlatTable>(name_+"TRANSFER_triangle_transfered");

    produces<nanoaod::FlatTable>(name_+"TRANSFER_dipole_unmatched_reco");
    produces<nanoaod::FlatTable>(name_+"TRANSFER_tee_unmatched_reco");
    produces<nanoaod::FlatTable>(name_+"TRANSFER_triangle_unmatched_reco");

    produces<nanoaod::FlatTable>(name_+"TRANSFER_dipole_unmatched_gen");
    produces<nanoaod::FlatTable>(name_+"TRANSFER_tee_unmatched_gen");
    produces<nanoaod::FlatTable>(name_+"TRANSFER_triangle_unmatched_gen");

    produces<nanoaod::FlatTable>(name_+"BK");
}

void EECRes4TransferTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    desc.add<std::string>("name");
    desc.add<edm::InputTag>("EECTransfer");
    descriptions.add("EECRes4TransferTableProducer", desc);
}

void EECRes4TransferTableProducer::produce(edm::Event& event, const edm::EventSetup& setup){
    edm::Handle<std::vector<EEC::CMSSW_Res4TransferResult>> EECTransfer_vec;
    event.getByToken(EECTransferToken_, EECTransfer_vec);

    std::vector<int> transfered_R_dipole_reco, transfered_r_dipole_reco, transfered_c_dipole_reco;
    std::vector<int> transfered_R_dipole_gen, transfered_r_dipole_gen, transfered_c_dipole_gen;
    std::vector<float> transfered_dipole_wt;

    std::vector<int> transfered_R_tee_reco, transfered_r_tee_reco, transfered_c_tee_reco;
    std::vector<int> transfered_R_tee_gen, transfered_r_tee_gen, transfered_c_tee_gen;
    std::vector<float> transfered_tee_wt;

    std::vector<int> transfered_R_triangle_reco, transfered_r_triangle_reco, transfered_c_triangle_reco;
    std::vector<int> transfered_R_triangle_gen, transfered_r_triangle_gen, transfered_c_triangle_gen;
    std::vector<float> transfered_triangle_wt;

    std::vector<float> dipole_flat_unmatched_reco;
    std::vector<float> tee_flat_unmatched_reco;
    std::vector<float> triangle_flat_unmatched_reco;

    std::vector<float> dipole_flat_unmatched_gen;
    std::vector<float> tee_flat_unmatched_gen;
    std::vector<float> triangle_flat_unmatched_gen;

    std::vector<int> nR_dipole_reco, nr_dipole_reco, nc_dipole_reco;
    std::vector<int> nR_tee_reco, nr_tee_reco, nc_tee_reco;
    std::vector<int> nR_triangle_reco, nr_triangle_reco, nc_triangle_reco;

    std::vector<int> nR_dipole_gen, nr_dipole_gen, nc_dipole_gen;
    std::vector<int> nR_tee_gen, nr_tee_gen, nc_tee_gen;
    std::vector<int> nR_triangle_gen, nr_triangle_gen, nc_triangle_gen;

    std::vector<int> iReco, iGen;
    std::vector<float> pt_denom_reco, pt_denom_gen;

    for (const auto& EEC : *EECTransfer_vec){
        const auto& dipole = EEC.result.get_dipole();
        const auto& tee = EEC.result.get_tee();
        const auto& triangle = EEC.result.get_triangle();

        nR_dipole_gen.push_back(dipole.nR_gen);
        nr_dipole_gen.push_back(dipole.nr_gen);
        nc_dipole_gen.push_back(dipole.nc_gen);

        nR_tee_gen.push_back(tee.nR_gen);
        nr_tee_gen.push_back(tee.nr_gen);
        nc_tee_gen.push_back(tee.nc_gen);

        nR_triangle_gen.push_back(triangle.nR_gen);
        nr_triangle_gen.push_back(triangle.nr_gen);
        nc_triangle_gen.push_back(triangle.nc_gen);

        nR_dipole_reco.push_back(dipole.nR_reco);
        nr_dipole_reco.push_back(dipole.nr_reco);
        nc_dipole_reco.push_back(dipole.nc_reco);

        nR_tee_reco.push_back(tee.nR_reco);
        nr_tee_reco.push_back(tee.nr_reco);
        nc_tee_reco.push_back(tee.nc_reco);

        nR_triangle_reco.push_back(triangle.nR_reco);
        nr_triangle_reco.push_back(triangle.nr_reco);
        nc_triangle_reco.push_back(triangle.nc_reco);

        iReco.push_back(EEC.iReco);
        iGen.push_back(EEC.iGen);

        pt_denom_reco.push_back(EEC.result.get_pt_denom_reco());
        pt_denom_gen.push_back(EEC.result.get_pt_denom_gen());

        const auto& dipole_transfered = dipole.get_data();
        const auto& tee_transfered = tee.get_data();
        const auto& triangle_transfered = triangle.get_data();

        for (const auto& entry : dipole_transfered){
            transfered_R_dipole_reco.push_back(entry.iR_reco);
            transfered_r_dipole_reco.push_back(entry.ir_reco);
            transfered_c_dipole_reco.push_back(entry.ic_reco);
            transfered_R_dipole_gen.push_back(entry.iR_gen);
            transfered_r_dipole_gen.push_back(entry.ir_gen);
            transfered_c_dipole_gen.push_back(entry.ic_gen);
            transfered_dipole_wt.push_back(entry.wt);
        }

        for (const auto& entry : tee_transfered){
            transfered_R_tee_reco.push_back(entry.iR_reco);
            transfered_r_tee_reco.push_back(entry.ir_reco);
            transfered_c_tee_reco.push_back(entry.ic_reco);
            transfered_R_tee_gen.push_back(entry.iR_gen);
            transfered_r_tee_gen.push_back(entry.ir_gen);
            transfered_c_tee_gen.push_back(entry.ic_gen);
            transfered_tee_wt.push_back(entry.wt);
        }

        for (const auto& entry : triangle_transfered){
            transfered_R_triangle_reco.push_back(entry.iR_reco);
            transfered_r_triangle_reco.push_back(entry.ir_reco);
            transfered_c_triangle_reco.push_back(entry.ic_reco);
            transfered_R_triangle_gen.push_back(entry.iR_gen);
            transfered_r_triangle_gen.push_back(entry.ir_gen);
            transfered_c_triangle_gen.push_back(entry.ic_gen);
            transfered_triangle_wt.push_back(entry.wt);
        }

        const auto& dipole_unmatched_reco = EEC.result.unmatched_reco.get_dipole();
        const auto& tee_unmatched_reco = EEC.result.unmatched_reco.get_tee();
        const auto& triangle_unmatched_reco = EEC.result.unmatched_reco.get_triangle();

        dipole_flat_unmatched_reco.insert(
                dipole_flat_unmatched_reco.end(), 
                dipole_unmatched_reco.get_data().data(),
                dipole_unmatched_reco.get_data().data() + dipole_unmatched_reco.get_data().num_elements());

        tee_flat_unmatched_reco.insert(
                tee_flat_unmatched_reco.end(), 
                tee_unmatched_reco.get_data().data(),
                tee_unmatched_reco.get_data().data() + tee_unmatched_reco.get_data().num_elements());

        triangle_flat_unmatched_reco.insert(
                triangle_flat_unmatched_reco.end(), 
                triangle_unmatched_reco.get_data().data(),
                triangle_unmatched_reco.get_data().data() + triangle_unmatched_reco.get_data().num_elements());

        const auto& dipole_unmatched_gen = EEC.result.unmatched_gen.get_dipole();
        const auto& tee_unmatched_gen = EEC.result.unmatched_gen.get_tee();
        const auto& triangle_unmatched_gen = EEC.result.unmatched_gen.get_triangle();

        dipole_flat_unmatched_gen.insert(
                dipole_flat_unmatched_gen.end(), 
                dipole_unmatched_gen.get_data().data(),
                dipole_unmatched_gen.get_data().data() + dipole_unmatched_gen.get_data().num_elements());

        tee_flat_unmatched_gen.insert(
                tee_flat_unmatched_gen.end(), 
                tee_unmatched_gen.get_data().data(),
                tee_unmatched_gen.get_data().data() + tee_unmatched_gen.get_data().num_elements());

        triangle_flat_unmatched_gen.insert(
                triangle_flat_unmatched_gen.end(), 
                triangle_unmatched_gen.get_data().data(),
                triangle_unmatched_gen.get_data().data() + triangle_unmatched_gen.get_data().num_elements());
    }

    auto dipoleTransferTable = std::make_unique<nanoaod::FlatTable>(transfered_dipole_wt.size(), name_+"TRANSFER_dipole_transfered", false);
    dipoleTransferTable->addColumn<int>("R_reco", transfered_R_dipole_reco, "reco R index");
    dipoleTransferTable->addColumn<int>("r_reco", transfered_r_dipole_reco, "reco r index");
    dipoleTransferTable->addColumn<int>("c_reco", transfered_c_dipole_reco, "reco c index");
    dipoleTransferTable->addColumn<int>("R_gen", transfered_R_dipole_gen, "gen R index");
    dipoleTransferTable->addColumn<int>("r_gen", transfered_r_dipole_gen, "gen r index");
    dipoleTransferTable->addColumn<int>("c_gen", transfered_c_dipole_gen, "gen c index");
    dipoleTransferTable->addColumn<float>("wt", transfered_dipole_wt, "weight");
    event.put(std::move(dipoleTransferTable), name_+"TRANSFER_dipole_transfered");

    auto teeTransferTable = std::make_unique<nanoaod::FlatTable>(transfered_tee_wt.size(), name_+"TRANSFER_tee_transfered", false);
    teeTransferTable->addColumn<int>("R_reco", transfered_R_tee_reco, "reco R index");
    teeTransferTable->addColumn<int>("r_reco", transfered_r_tee_reco, "reco r index");
    teeTransferTable->addColumn<int>("c_reco", transfered_c_tee_reco, "reco c index");
    teeTransferTable->addColumn<int>("R_gen", transfered_R_tee_gen, "gen R index");
    teeTransferTable->addColumn<int>("r_gen", transfered_r_tee_gen, "gen r index");
    teeTransferTable->addColumn<int>("c_gen", transfered_c_tee_gen, "gen c index");
    teeTransferTable->addColumn<float>("wt", transfered_tee_wt, "weight");
    event.put(std::move(teeTransferTable), name_+"TRANSFER_tee_transfered");

    auto triangleTransferTable = std::make_unique<nanoaod::FlatTable>(transfered_triangle_wt.size(), name_+"TRANSFER_triangle_transfered", false);
    triangleTransferTable->addColumn<int>("R_reco", transfered_R_triangle_reco, "reco R index");
    triangleTransferTable->addColumn<int>("r_reco", transfered_r_triangle_reco, "reco r index");
    triangleTransferTable->addColumn<int>("c_reco", transfered_c_triangle_reco, "reco c index");
    triangleTransferTable->addColumn<int>("R_gen", transfered_R_triangle_gen, "gen R index");
    triangleTransferTable->addColumn<int>("r_gen", transfered_r_triangle_gen, "gen r index");
    triangleTransferTable->addColumn<int>("c_gen", transfered_c_triangle_gen, "gen c index");
    triangleTransferTable->addColumn<int>("wt", transfered_triangle_wt, "wt");
    event.put(std::move(triangleTransferTable), name_+"TRANSFER_triangle_transfered");

    auto dipoleUnmatchedRecoTable = std::make_unique<nanoaod::FlatTable>(dipole_flat_unmatched_reco.size(), name_+"TRANSFER_dipole_unmatched_reco", false);
    dipoleUnmatchedRecoTable->addColumn<float>("value", dipole_flat_unmatched_reco, "dipole values");
    event.put(std::move(dipoleUnmatchedRecoTable), name_+"TRANSFER_dipole_unmatched_reco");

    auto teeUnmatchedRecoTable = std::make_unique<nanoaod::FlatTable>(tee_flat_unmatched_reco.size(), name_+"TRANSFER_tee_unmatched_reco", false);
    teeUnmatchedRecoTable->addColumn<float>("value", tee_flat_unmatched_reco, "tee values");
    event.put(std::move(teeUnmatchedRecoTable), name_+"TRANSFER_tee_unmatched_reco");

    auto triangleUnmatchedRecoTable = std::make_unique<nanoaod::FlatTable>(triangle_flat_unmatched_reco.size(), name_+"TRANSFER_triangle_unmatched_reco", false);
    triangleUnmatchedRecoTable->addColumn<float>("value", triangle_flat_unmatched_reco, "triangle values");
    event.put(std::move(triangleUnmatchedRecoTable), name_+"TRANSFER_triangle_unmatched_reco");

    auto dipoleUnmatchedGenTable = std::make_unique<nanoaod::FlatTable>(dipole_flat_unmatched_gen.size(), name_+"TRANSFER_dipole_unmatched_gen", false);
    dipoleUnmatchedGenTable->addColumn<float>("value", dipole_flat_unmatched_gen, "dipole values");
    event.put(std::move(dipoleUnmatchedGenTable), name_+"TRANSFER_dipole_unmatched_gen");

    auto teeUnmatchedGenTable = std::make_unique<nanoaod::FlatTable>(tee_flat_unmatched_gen.size(), name_+"TRANSFER_tee_unmatched_gen", false);
    teeUnmatchedGenTable->addColumn<float>("value", tee_flat_unmatched_gen, "tee values");
    event.put(std::move(teeUnmatchedGenTable), name_+"TRANSFER_tee_unmatched_gen");

    auto triangleUnmatchedGenTable = std::make_unique<nanoaod::FlatTable>(triangle_flat_unmatched_gen.size(), name_+"TRANSFER_triangle_unmatched_gen", false);
    triangleUnmatchedGenTable->addColumn<float>("value", triangle_flat_unmatched_gen, "triangle values");
    event.put(std::move(triangleUnmatchedGenTable), name_+"TRANSFER_triangle_unmatched_gen");

    auto BKTable = std::make_unique<nanoaod::FlatTable>(EECTransfer_vec->size(), name_+"BK", false);
    BKTable->addColumn<int>("nR_dipole_reco", nR_dipole_reco, "nR dipole reco");
    BKTable->addColumn<int>("nr_dipole_reco", nr_dipole_reco, "nr dipole reco");
    BKTable->addColumn<int>("nc_dipole_reco", nc_dipole_reco, "nc dipole reco");
    BKTable->addColumn<int>("nR_tee_reco", nR_tee_reco, "nR tee reco");
    BKTable->addColumn<int>("nr_tee_reco", nr_tee_reco, "nr tee reco");
    BKTable->addColumn<int>("nc_tee_reco", nc_tee_reco, "nc tee reco");
    BKTable->addColumn<int>("nR_triangle_reco", nR_triangle_reco, "nR triangle reco");
    BKTable->addColumn<int>("nr_triangle_reco", nr_triangle_reco, "nr triangle reco");
    BKTable->addColumn<int>("nc_triangle_reco", nc_triangle_reco, "nc triangle reco");
    BKTable->addColumn<int>("nR_dipole_gen", nR_dipole_gen, "nR dipole gen");
    BKTable->addColumn<int>("nr_dipole_gen", nr_dipole_gen, "nr dipole gen");
    BKTable->addColumn<int>("nc_dipole_gen", nc_dipole_gen, "nc dipole gen");
    BKTable->addColumn<int>("nR_tee_gen", nR_tee_gen, "nR tee gen");
    BKTable->addColumn<int>("nr_tee_gen", nr_tee_gen, "nr tee gen");
    BKTable->addColumn<int>("nc_tee_gen", nc_tee_gen, "nc tee gen");
    BKTable->addColumn<int>("nR_triangle_gen", nR_triangle_gen, "nR triangle gen");
    BKTable->addColumn<int>("nr_triangle_gen", nr_triangle_gen, "nr triangle gen");
    BKTable->addColumn<int>("nc_triangle_gen", nc_triangle_gen, "nc triangle gen");
    BKTable->addColumn<int>("iReco", iReco, "reco jet index");
    BKTable->addColumn<int>("iGen", iGen, "gen jet index");
    BKTable->addColumn<float>("pt_denom_reco", pt_denom_reco, "reco jet pt denominator");
    BKTable->addColumn<float>("pt_denom_gen", pt_denom_gen, "gen jet pt denominator");
    event.put(std::move(BKTable), name_+"BK");
}
