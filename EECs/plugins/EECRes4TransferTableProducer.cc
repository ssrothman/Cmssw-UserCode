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

#include "SRothman/EECs/src/Res4TransferResult.h"

template <class ResultType>
class EECRes4TransferTableProducer : public edm::stream::EDProducer<> {
public:
    explicit EECRes4TransferTableProducer(const edm::ParameterSet&);
    ~EECRes4TransferTableProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    std::string name_;

    edm::EDGetTokenT<std::vector<ResultType>> EECTransferToken_;
};

template <class ResultType>
EECRes4TransferTableProducer<ResultType>::EECRes4TransferTableProducer(const edm::ParameterSet& conf) :
        name_(conf.getParameter<std::string>("name")),
        EECTransferToken_(consumes<std::vector<ResultType>>(conf.getParameter<edm::InputTag>("EECTransfer"))) {

    produces<nanoaod::FlatTable>(name_+"dipole");
    produces<nanoaod::FlatTable>(name_+"tee");
    produces<nanoaod::FlatTable>(name_+"triangle");

    produces<nanoaod::FlatTable>(name_+"BK");
}

template <class ResultType>
void EECRes4TransferTableProducer<ResultType>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    desc.add<std::string>("name");
    desc.add<edm::InputTag>("EECTransfer");
    descriptions.addWithDefaultLabel(desc);
}

template <class ResultType>
void EECRes4TransferTableProducer<ResultType>::produce(edm::Event& event, const edm::EventSetup& setup){
    edm::Handle<std::vector<ResultType>> EECTransfer_vec;
    event.getByToken(EECTransferToken_, EECTransfer_vec);

    std::vector<typename ResultType::T> transfered_R_dipole_reco, transfered_r_dipole_reco, transfered_c_dipole_reco;
    std::vector<typename ResultType::T> transfered_R_dipole_gen, transfered_r_dipole_gen, transfered_c_dipole_gen;
    std::vector<float> transfered_dipole_wt_reco, transfered_dipole_wt_gen;

    std::vector<typename ResultType::T> transfered_R_tee_reco, transfered_r_tee_reco, transfered_c_tee_reco;
    std::vector<typename ResultType::T> transfered_R_tee_gen, transfered_r_tee_gen, transfered_c_tee_gen;
    std::vector<float> transfered_tee_wt_reco, transfered_tee_wt_gen;

    std::vector<typename ResultType::T> transfered_R_triangle_reco, transfered_r_triangle_reco, transfered_c_triangle_reco;
    std::vector<typename ResultType::T> transfered_R_triangle_gen, transfered_r_triangle_gen, transfered_c_triangle_gen;
    std::vector<float> transfered_triangle_wt_reco, transfered_triangle_wt_gen;

    std::vector<int> nR_dipole_reco, nr_dipole_reco, nc_dipole_reco;
    std::vector<int> nR_tee_reco, nr_tee_reco, nc_tee_reco;
    std::vector<int> nR_triangle_reco, nr_triangle_reco, nc_triangle_reco;

    std::vector<int> nR_dipole_gen, nr_dipole_gen, nc_dipole_gen;
    std::vector<int> nR_tee_gen, nr_tee_gen, nc_tee_gen;
    std::vector<int> nR_triangle_gen, nr_triangle_gen, nc_triangle_gen;

    std::vector<int> nEntries_dipole, nEntries_tee, nEntries_triangle;

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
            
        if constexpr (ResultType::IS_ARRAY){
            int entries_dipole = 0;
            for (unsigned iR_reco=0; iR_reco < dipole_transfered.shape()[0]; ++iR_reco){
                for(unsigned ir_reco=0; ir_reco < dipole_transfered.shape()[1]; ++ir_reco){
                    for(unsigned ic_reco=0; ic_reco < dipole_transfered.shape()[2]; ++ic_reco){
                        for(unsigned iR_gen=0; iR_gen < dipole_transfered.shape()[3]; ++iR_gen){
                            for(unsigned ir_gen=0; ir_gen < dipole_transfered.shape()[4]; ++ir_gen){
                                for(unsigned ic_gen=0; ic_gen < dipole_transfered.shape()[5]; ++ic_gen){
                                    double value = dipole_transfered[iR_reco][ir_reco][ic_reco][iR_gen][ir_gen][ic_gen];
                                    if (value > 0){
                                        entries_dipole++;
                                        transfered_R_dipole_reco.push_back(iR_reco);
                                        transfered_r_dipole_reco.push_back(ir_reco);
                                        transfered_c_dipole_reco.push_back(ic_reco);
                                        transfered_R_dipole_gen.push_back(iR_gen);
                                        transfered_r_dipole_gen.push_back(ir_gen);
                                        transfered_c_dipole_gen.push_back(ic_gen);
                                        transfered_dipole_wt_reco.push_back(value);
                                        transfered_dipole_wt_gen.push_back(value); //THIS IS WRONG
                                                                                   //BUT THE ARRAY DOES NOT SAVE THE GEN WEIGHT
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (entries_dipole == 0){
                nEntries_dipole.push_back(1);
                transfered_R_dipole_reco.push_back(-1);
                transfered_r_dipole_reco.push_back(-1);
                transfered_c_dipole_reco.push_back(-1);
                transfered_R_dipole_gen.push_back(-1);
                transfered_r_dipole_gen.push_back(-1);
                transfered_c_dipole_gen.push_back(-1);
                transfered_dipole_wt_reco.push_back(-1);
                transfered_dipole_wt_gen.push_back(-1);
            } else {
                nEntries_dipole.push_back(entries_dipole);
            }

            int entries_tee = 0;
            for (unsigned iR_reco=0; iR_reco < tee_transfered.shape()[0]; ++iR_reco){
                for(unsigned ir_reco=0; ir_reco < tee_transfered.shape()[1]; ++ir_reco){
                    for(unsigned ic_reco=0; ic_reco < tee_transfered.shape()[2]; ++ic_reco){
                        for(unsigned iR_gen=0; iR_gen < tee_transfered.shape()[3]; ++iR_gen){
                            for(unsigned ir_gen=0; ir_gen < tee_transfered.shape()[4]; ++ir_gen){
                                for(unsigned ic_gen=0; ic_gen < tee_transfered.shape()[5]; ++ic_gen){
                                    double value = tee_transfered[iR_reco][ir_reco][ic_reco][iR_gen][ir_gen][ic_gen];
                                    if (value > 0){
                                        entries_tee++;
                                        transfered_R_tee_reco.push_back(iR_reco);
                                        transfered_r_tee_reco.push_back(ir_reco);
                                        transfered_c_tee_reco.push_back(ic_reco);
                                        transfered_R_tee_gen.push_back(iR_gen);
                                        transfered_r_tee_gen.push_back(ir_gen);
                                        transfered_c_tee_gen.push_back(ic_gen);
                                        transfered_tee_wt_reco.push_back(value);
                                        transfered_tee_wt_gen.push_back(value); //THIS IS WRONG
                                                                                   //BUT THE ARRAY DOES NOT SAVE THE GEN WEIGHT
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (entries_tee == 0){
                nEntries_tee.push_back(1);
                transfered_R_tee_reco.push_back(-1);
                transfered_r_tee_reco.push_back(-1);
                transfered_c_tee_reco.push_back(-1);
                transfered_R_tee_gen.push_back(-1);
                transfered_r_tee_gen.push_back(-1);
                transfered_c_tee_gen.push_back(-1);
                transfered_tee_wt_reco.push_back(-1);
                transfered_tee_wt_gen.push_back(-1);
            } else {
                nEntries_tee.push_back(entries_tee);
            }

            int entries_triangle = 0;
            for (unsigned iR_reco=0; iR_reco < triangle_transfered.shape()[0]; ++iR_reco){
                for(unsigned ir_reco=0; ir_reco < triangle_transfered.shape()[1]; ++ir_reco){
                    for(unsigned ic_reco=0; ic_reco < triangle_transfered.shape()[2]; ++ic_reco){
                        for(unsigned iR_gen=0; iR_gen < triangle_transfered.shape()[3]; ++iR_gen){
                            for(unsigned ir_gen=0; ir_gen < triangle_transfered.shape()[4]; ++ir_gen){
                                for(unsigned ic_gen=0; ic_gen < triangle_transfered.shape()[5]; ++ic_gen){
                                    double value = triangle_transfered[iR_reco][ir_reco][ic_reco][iR_gen][ir_gen][ic_gen];
                                    if (value > 0){
                                        entries_triangle++;
                                        transfered_R_triangle_reco.push_back(iR_reco);
                                        transfered_r_triangle_reco.push_back(ir_reco);
                                        transfered_c_triangle_reco.push_back(ic_reco);
                                        transfered_R_triangle_gen.push_back(iR_gen);
                                        transfered_r_triangle_gen.push_back(ir_gen);
                                        transfered_c_triangle_gen.push_back(ic_gen);
                                        transfered_triangle_wt_reco.push_back(value);
                                        transfered_triangle_wt_gen.push_back(value); //THIS IS WRONG
                                                                                       //BUT THE ARRAY DOES NOT SAVE THE GEN WEIGHT
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (entries_triangle == 0){
                nEntries_triangle.push_back(1);
                transfered_R_triangle_reco.push_back(-1);
                transfered_r_triangle_reco.push_back(-1);
                transfered_c_triangle_reco.push_back(-1);
                transfered_R_triangle_gen.push_back(-1);
                transfered_r_triangle_gen.push_back(-1);
                transfered_c_triangle_gen.push_back(-1);
                transfered_triangle_wt_reco.push_back(-1);
                transfered_triangle_wt_gen.push_back(-1);
            } else {
                nEntries_triangle.push_back(entries_triangle);
            }
        } else {
            if (dipole_transfered.size() > 0){
                nEntries_dipole.push_back(dipole_transfered.size());
                for (const auto& entry : dipole_transfered){
                    transfered_R_dipole_reco.push_back(entry.iR_reco);
                    transfered_r_dipole_reco.push_back(entry.ir_reco);
                    transfered_c_dipole_reco.push_back(entry.ic_reco);
                    transfered_R_dipole_gen.push_back(entry.iR_gen);
                    transfered_r_dipole_gen.push_back(entry.ir_gen);
                    transfered_c_dipole_gen.push_back(entry.ic_gen);
                    transfered_dipole_wt_reco.push_back(entry.wt_reco);
                    transfered_dipole_wt_gen.push_back(entry.wt_gen);
                }
            } else {
                nEntries_dipole.push_back(1);
                transfered_R_dipole_reco.push_back(-1);
                transfered_r_dipole_reco.push_back(-1);
                transfered_c_dipole_reco.push_back(-1);
                transfered_R_dipole_gen.push_back(-1);
                transfered_r_dipole_gen.push_back(-1);
                transfered_c_dipole_gen.push_back(-1);
                transfered_dipole_wt_reco.push_back(-1);
                transfered_dipole_wt_gen.push_back(-1);
            }

            if(tee_transfered.size() > 0){
                nEntries_tee.push_back(tee_transfered.size());
                for (const auto& entry : tee_transfered){
                    transfered_R_tee_reco.push_back(entry.iR_reco);
                    transfered_r_tee_reco.push_back(entry.ir_reco);
                    transfered_c_tee_reco.push_back(entry.ic_reco);
                    transfered_R_tee_gen.push_back(entry.iR_gen);
                    transfered_r_tee_gen.push_back(entry.ir_gen);
                    transfered_c_tee_gen.push_back(entry.ic_gen);
                    transfered_tee_wt_reco.push_back(entry.wt_reco);
                    transfered_tee_wt_gen.push_back(entry.wt_gen);
                }
            } else {
                nEntries_tee.push_back(1);
                transfered_R_tee_reco.push_back(-1);
                transfered_r_tee_reco.push_back(-1);
                transfered_c_tee_reco.push_back(-1);
                transfered_R_tee_gen.push_back(-1);
                transfered_r_tee_gen.push_back(-1);
                transfered_c_tee_gen.push_back(-1);
                transfered_tee_wt_reco.push_back(-1);
                transfered_tee_wt_gen.push_back(-1);
            }

            if(triangle_transfered.size() > 0){
                nEntries_triangle.push_back(triangle_transfered.size());
                for (const auto& entry : triangle_transfered){
                    transfered_R_triangle_reco.push_back(entry.iR_reco);
                    transfered_r_triangle_reco.push_back(entry.ir_reco);
                    transfered_c_triangle_reco.push_back(entry.ic_reco);
                    transfered_R_triangle_gen.push_back(entry.iR_gen);
                    transfered_r_triangle_gen.push_back(entry.ir_gen);
                    transfered_c_triangle_gen.push_back(entry.ic_gen);
                    transfered_triangle_wt_reco.push_back(entry.wt_reco);
                    transfered_triangle_wt_gen.push_back(entry.wt_gen);
                }
            } else {
                nEntries_triangle.push_back(1);
                transfered_R_triangle_reco.push_back(-1);
                transfered_r_triangle_reco.push_back(-1);
                transfered_c_triangle_reco.push_back(-1);
                transfered_R_triangle_gen.push_back(-1);
                transfered_r_triangle_gen.push_back(-1);
                transfered_c_triangle_gen.push_back(-1);
                transfered_triangle_wt_reco.push_back(-1);
                transfered_triangle_wt_gen.push_back(-1);
            }
        }
    }

    auto dipoleTransferTable = std::make_unique<nanoaod::FlatTable>(transfered_dipole_wt_reco.size(), name_+"dipole", false);
    dipoleTransferTable->addColumn<typename ResultType::T>("R_reco", transfered_R_dipole_reco, "reco R index", ResultType::COLUMN_TYPE);
    dipoleTransferTable->template addColumn<typename ResultType::T>("r_reco", transfered_r_dipole_reco, "reco r index", ResultType::COLUMN_TYPE);
    dipoleTransferTable->template addColumn<typename ResultType::T>("c_reco", transfered_c_dipole_reco, "reco c index", ResultType::COLUMN_TYPE);
    dipoleTransferTable->template addColumn<typename ResultType::T>("R_gen", transfered_R_dipole_gen, "gen R index",    ResultType::COLUMN_TYPE);
    dipoleTransferTable->template addColumn<typename ResultType::T>("r_gen", transfered_r_dipole_gen, "gen r index",    ResultType::COLUMN_TYPE);
    dipoleTransferTable->template addColumn<typename ResultType::T>("c_gen", transfered_c_dipole_gen, "gen c index",    ResultType::COLUMN_TYPE);
    dipoleTransferTable->addColumn<float>("wt_reco", transfered_dipole_wt_reco, "weight", nanoaod::FlatTable::FloatColumn);
    dipoleTransferTable->addColumn<float>("wt_gen", transfered_dipole_wt_gen, "weight", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(dipoleTransferTable), name_+"dipole");

    auto teeTransferTable = std::make_unique<nanoaod::FlatTable>(transfered_tee_wt_reco.size(), name_+"tee", false);
    teeTransferTable->template addColumn<typename ResultType::T>("R_reco", transfered_R_tee_reco, "reco R index", ResultType::COLUMN_TYPE);
    teeTransferTable->template addColumn<typename ResultType::T>("r_reco", transfered_r_tee_reco, "reco r index", ResultType::COLUMN_TYPE);
    teeTransferTable->template addColumn<typename ResultType::T>("c_reco", transfered_c_tee_reco, "reco c index", ResultType::COLUMN_TYPE);
    teeTransferTable->template addColumn<typename ResultType::T>("R_gen", transfered_R_tee_gen, "gen R index",    ResultType::COLUMN_TYPE);
    teeTransferTable->template addColumn<typename ResultType::T>("r_gen", transfered_r_tee_gen, "gen r index",    ResultType::COLUMN_TYPE);
    teeTransferTable->template addColumn<typename ResultType::T>("c_gen", transfered_c_tee_gen, "gen c index",    ResultType::COLUMN_TYPE);
    teeTransferTable->addColumn<float>("wt_reco", transfered_tee_wt_reco, "weight", nanoaod::FlatTable::FloatColumn);
    teeTransferTable->addColumn<float>("wt_gen", transfered_tee_wt_gen, "weight", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(teeTransferTable), name_+"tee");

    auto triangleTransferTable = std::make_unique<nanoaod::FlatTable>(transfered_triangle_wt_reco.size(), name_+"triangle", false);
    triangleTransferTable->template addColumn<typename ResultType::T>("R_reco", transfered_R_triangle_reco, "reco R index", ResultType::COLUMN_TYPE);
    triangleTransferTable->template addColumn<typename ResultType::T>("r_reco", transfered_r_triangle_reco, "reco r index", ResultType::COLUMN_TYPE);
    triangleTransferTable->template addColumn<typename ResultType::T>("c_reco", transfered_c_triangle_reco, "reco c index", ResultType::COLUMN_TYPE);
    triangleTransferTable->template addColumn<typename ResultType::T>("R_gen", transfered_R_triangle_gen, "gen R index",    ResultType::COLUMN_TYPE);
    triangleTransferTable->template addColumn<typename ResultType::T>("r_gen", transfered_r_triangle_gen, "gen r index",    ResultType::COLUMN_TYPE);
    triangleTransferTable->template addColumn<typename ResultType::T>("c_gen", transfered_c_triangle_gen, "gen c index",    ResultType::COLUMN_TYPE);
    triangleTransferTable->addColumn<float>("wt_reco", transfered_triangle_wt_reco, "wt_reco",    nanoaod::FlatTable::FloatColumn);
    triangleTransferTable->addColumn<float>("wt_gen", transfered_triangle_wt_gen, "wt_gen", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(triangleTransferTable), name_+"triangle");

    auto BKTable = std::make_unique<nanoaod::FlatTable>(EECTransfer_vec->size(), name_+"BK", false);
    BKTable->template addColumn<int>("nR_dipole_reco", nR_dipole_reco, "nR dipole reco",          nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nr_dipole_reco", nr_dipole_reco, "nr dipole reco",          nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nc_dipole_reco", nc_dipole_reco, "nc dipole reco",          nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nR_tee_reco", nR_tee_reco, "nR tee reco",                   nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nr_tee_reco", nr_tee_reco, "nr tee reco",                   nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nc_tee_reco", nc_tee_reco, "nc tee reco",                   nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nR_triangle_reco", nR_triangle_reco, "nR triangle reco",    nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nr_triangle_reco", nr_triangle_reco, "nr triangle reco",    nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nc_triangle_reco", nc_triangle_reco, "nc triangle reco",    nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nR_dipole_gen", nR_dipole_gen, "nR dipole gen",             nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nr_dipole_gen", nr_dipole_gen, "nr dipole gen",             nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nc_dipole_gen", nc_dipole_gen, "nc dipole gen",             nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nR_tee_gen", nR_tee_gen, "nR tee gen",                      nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nr_tee_gen", nr_tee_gen, "nr tee gen",                      nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nc_tee_gen", nc_tee_gen, "nc tee gen",                      nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nR_triangle_gen", nR_triangle_gen, "nR triangle gen",       nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nr_triangle_gen", nr_triangle_gen, "nr triangle gen",       nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nc_triangle_gen", nc_triangle_gen, "nc triangle gen",       nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nEntries_dipole", nEntries_dipole, "nEntries dipole",       nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nEntries_tee", nEntries_tee, "nEntries tee",                nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nEntries_triangle", nEntries_triangle, "nEntries triangle", nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("iReco", iReco, "reco jet index",                            nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("iGen", iGen, "gen jet index",                               nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<float>("pt_denom_reco", pt_denom_reco, "reco jet pt denominator", nanoaod::FlatTable::FloatColumn);
    BKTable->template addColumn<float>("pt_denom_gen", pt_denom_gen, "gen jet pt denominator",    nanoaod::FlatTable::FloatColumn);
    event.put(std::move(BKTable), name_+"BK");
}

typedef EECRes4TransferTableProducer<EEC::CMSSWTransferResult<EEC::Res4TransferResult<EEC::ResTransferVectorContainer<double>>>> EECRes4TransferUnbinnedTableProducer;
typedef EECRes4TransferTableProducer<EEC::CMSSWTransferResult<EEC::Res4TransferResult<EEC::ResTransferVectorContainer<unsigned>>>> EECRes4TransferVectorTableProducer;
typedef EECRes4TransferTableProducer<EEC::CMSSWTransferResult<EEC::Res4TransferResult<EEC::ResTransferMultiArrayContainer>>> EECRes4TransferArrayTableProducer;

DEFINE_FWK_MODULE(EECRes4TransferUnbinnedTableProducer);
DEFINE_FWK_MODULE(EECRes4TransferVectorTableProducer);
DEFINE_FWK_MODULE(EECRes4TransferArrayTableProducer);
