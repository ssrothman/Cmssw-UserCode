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

#include "SRothman/EECs/src/Res3TransferResult.h"

template <class ResultType>
class EECRes3TransferTableProducer : public edm::stream::EDProducer<> {
public:
    explicit EECRes3TransferTableProducer(const edm::ParameterSet&);
    ~EECRes3TransferTableProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    std::string name_;

    edm::EDGetTokenT<std::vector<ResultType>> EECTransferToken_;
};

template <class ResultType>
EECRes3TransferTableProducer<ResultType>::EECRes3TransferTableProducer(const edm::ParameterSet& conf) :
        name_(conf.getParameter<std::string>("name")),
        EECTransferToken_(consumes<std::vector<ResultType>>(conf.getParameter<edm::InputTag>("EECTransfer"))) {

    produces<nanoaod::FlatTable>(name_);

    produces<nanoaod::FlatTable>(name_+"BK");
}

template <class ResultType>
void EECRes3TransferTableProducer<ResultType>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    desc.add<std::string>("name");
    desc.add<edm::InputTag>("EECTransfer");
    descriptions.addWithDefaultLabel(desc);
}

template <class ResultType>
void EECRes3TransferTableProducer<ResultType>::produce(edm::Event& event, const edm::EventSetup& setup){
    edm::Handle<std::vector<ResultType>> EECTransfer_vec;
    event.getByToken(EECTransferToken_, EECTransfer_vec);

    std::vector<typename ResultType::T> transfered_R_reco, transfered_r_reco, transfered_c_reco;
    std::vector<typename ResultType::T> transfered_R_gen, transfered_r_gen, transfered_c_gen;
    std::vector<float> transfered_wt_reco, transfered_wt_gen;

    std::vector<int> nR_reco, nr_reco, nc_reco;

    std::vector<int> nR_gen, nr_gen, nc_gen;

    std::vector<int> nEntries;

    std::vector<int> iReco, iGen;
    std::vector<float> pt_denom_reco, pt_denom_gen;

    for (const auto& EEC : *EECTransfer_vec){
        const auto& data = EEC.result.get_data();

        nR_gen.push_back(data.nR_gen);
        nr_gen.push_back(data.nr_gen);
        nc_gen.push_back(data.nc_gen);

        nR_reco.push_back(data.nR_reco);
        nr_reco.push_back(data.nr_reco);
        nc_reco.push_back(data.nc_reco);

        iReco.push_back(EEC.iReco);
        iGen.push_back(EEC.iGen);

        pt_denom_reco.push_back(EEC.result.get_pt_denom_reco());
        pt_denom_gen.push_back(EEC.result.get_pt_denom_gen());

        const auto& data_transfered = data.get_data();
            
        if constexpr (ResultType::IS_ARRAY){
            int entries = 0;
            for (unsigned iR_reco=0; iR_reco < data_transfered.shape()[0]; ++iR_reco){
                for(unsigned ir_reco=0; ir_reco < data_transfered.shape()[1]; ++ir_reco){
                    for(unsigned ic_reco=0; ic_reco < data_transfered.shape()[2]; ++ic_reco){
                        for(unsigned iR_gen=0; iR_gen < data_transfered.shape()[3]; ++iR_gen){
                            for(unsigned ir_gen=0; ir_gen < data_transfered.shape()[4]; ++ir_gen){
                                for(unsigned ic_gen=0; ic_gen < data_transfered.shape()[5]; ++ic_gen){
                                    double value = data_transfered[iR_reco][ir_reco][ic_reco][iR_gen][ir_gen][ic_gen];
                                    if (value > 0){
                                        entries++;
                                        transfered_R_reco.push_back(iR_reco);
                                        transfered_r_reco.push_back(ir_reco);
                                        transfered_c_reco.push_back(ic_reco);
                                        transfered_R_gen.push_back(iR_gen);
                                        transfered_r_gen.push_back(ir_gen);
                                        transfered_c_gen.push_back(ic_gen);
                                        transfered_wt_reco.push_back(value);
                                        transfered_wt_gen.push_back(value); //THIS IS WRONG
                                                                                   //BUT THE ARRAY DOES NOT SAVE THE GEN WEIGHT
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (entries == 0){
                nEntries.push_back(1);
                transfered_R_reco.push_back(-1);
                transfered_r_reco.push_back(-1);
                transfered_c_reco.push_back(-1);
                transfered_R_gen.push_back(-1);
                transfered_r_gen.push_back(-1);
                transfered_c_gen.push_back(-1);
                transfered_wt_reco.push_back(-1);
                transfered_wt_gen.push_back(-1);
            } else {
                nEntries.push_back(entries);
            }
        } else {
            if (data_transfered.size() > 0){
                nEntries.push_back(data_transfered.size());
                for (const auto& entry : data_transfered){
                    transfered_R_reco.push_back(entry.iR_reco);
                    transfered_r_reco.push_back(entry.ir_reco);
                    transfered_c_reco.push_back(entry.ic_reco);
                    transfered_R_gen.push_back(entry.iR_gen);
                    transfered_r_gen.push_back(entry.ir_gen);
                    transfered_c_gen.push_back(entry.ic_gen);
                    transfered_wt_reco.push_back(entry.wt_reco);
                    transfered_wt_gen.push_back(entry.wt_gen);
                }
            } else {
                nEntries.push_back(1);
                transfered_R_reco.push_back(-1);
                transfered_r_reco.push_back(-1);
                transfered_c_reco.push_back(-1);
                transfered_R_gen.push_back(-1);
                transfered_r_gen.push_back(-1);
                transfered_c_gen.push_back(-1);
                transfered_wt_reco.push_back(-1);
                transfered_wt_gen.push_back(-1);
            }
        }
    }

    auto dataTransferTable = std::make_unique<nanoaod::FlatTable>(transfered_wt_reco.size(), name_, false);
    dataTransferTable->addColumn<typename ResultType::T>("R_reco", transfered_R_reco, "reco R index", ResultType::COLUMN_TYPE);
    dataTransferTable->template addColumn<typename ResultType::T>("r_reco", transfered_r_reco, "reco r index", ResultType::COLUMN_TYPE);
    dataTransferTable->template addColumn<typename ResultType::T>("c_reco", transfered_c_reco, "reco c index", ResultType::COLUMN_TYPE);
    dataTransferTable->template addColumn<typename ResultType::T>("R_gen", transfered_R_gen, "gen R index",    ResultType::COLUMN_TYPE);
    dataTransferTable->template addColumn<typename ResultType::T>("r_gen", transfered_r_gen, "gen r index",    ResultType::COLUMN_TYPE);
    dataTransferTable->template addColumn<typename ResultType::T>("c_gen", transfered_c_gen, "gen c index",    ResultType::COLUMN_TYPE);
    dataTransferTable->addColumn<float>("wt_reco", transfered_wt_reco, "weight", nanoaod::FlatTable::FloatColumn);
    dataTransferTable->addColumn<float>("wt_gen", transfered_wt_gen, "weight", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(dataTransferTable), name_);

    auto BKTable = std::make_unique<nanoaod::FlatTable>(EECTransfer_vec->size(), name_+"BK", false);
    BKTable->template addColumn<int>("nR_reco", nR_reco, "nR reco",          nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nr_reco", nr_reco, "nr reco",          nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nc_reco", nc_reco, "nc reco",          nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nR_gen", nR_gen, "nR gen",             nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nr_gen", nr_gen, "nr gen",             nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nc_gen", nc_gen, "nc gen",             nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("nEntries", nEntries, "nEntries",       nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("iReco", iReco, "reco jet index",                            nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<int>("iGen", iGen, "gen jet index",                               nanoaod::FlatTable::IntColumn);
    BKTable->template addColumn<float>("pt_denom_reco", pt_denom_reco, "reco jet pt denominator", nanoaod::FlatTable::FloatColumn);
    BKTable->template addColumn<float>("pt_denom_gen", pt_denom_gen, "gen jet pt denominator",    nanoaod::FlatTable::FloatColumn);
    event.put(std::move(BKTable), name_+"BK");
}

typedef EECRes3TransferTableProducer<EEC::CMSSWTransferResult<EEC::Res3TransferResult<EEC::ResTransferVectorContainer<double>>>> EECRes3TransferUnbinnedTableProducer;
typedef EECRes3TransferTableProducer<EEC::CMSSWTransferResult<EEC::Res3TransferResult<EEC::ResTransferVectorContainer<unsigned>>>> EECRes3TransferVectorTableProducer;
typedef EECRes3TransferTableProducer<EEC::CMSSWTransferResult<EEC::Res3TransferResult<EEC::ResTransferMultiArrayContainer>>> EECRes3TransferArrayTableProducer;

DEFINE_FWK_MODULE(EECRes3TransferUnbinnedTableProducer);
DEFINE_FWK_MODULE(EECRes3TransferVectorTableProducer);
DEFINE_FWK_MODULE(EECRes3TransferArrayTableProducer);

