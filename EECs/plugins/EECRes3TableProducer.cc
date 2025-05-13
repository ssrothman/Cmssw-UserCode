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

#include "SRothman/EECs/src/Res3Result.h"

template <class ResultType>
class EECRes3TableProducer : public edm::stream::EDProducer<> {
public:
    explicit EECRes3TableProducer(const edm::ParameterSet&);
    ~EECRes3TableProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    std::string name_;

    edm::EDGetTokenT<std::vector<ResultType>> EECToken_;
};

template <class ResultType>
EECRes3TableProducer<ResultType>::EECRes3TableProducer(const edm::ParameterSet& conf) :
        name_(conf.getParameter<std::string>("name")),
        EECToken_(consumes<std::vector<ResultType>>(conf.getParameter<edm::InputTag>("EECs"))) {

    produces<nanoaod::FlatTable>(name_);
    produces<nanoaod::FlatTable>(name_+"BK");
}

template <class ResultType>
void EECRes3TableProducer<ResultType>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    desc.add<std::string>("name");
    desc.add<edm::InputTag>("EECs");
    descriptions.addWithDefaultLabel(desc);
}

template <class ResultType>
void EECRes3TableProducer<ResultType>::produce(edm::Event& event, const edm::EventSetup& setup){
    edm::Handle<std::vector<ResultType>> EECs;
    event.getByToken(EECToken_, EECs);

    std::vector<typename ResultType::T> R, r, c;
    std::vector<float> wt;

    std::vector<int> nR, nr, nc;
    std::vector<int> nEntry;

    std::vector<int> iJet, iReco;
    std::vector<float> pt_denom;

    for (const auto& EEC : *EECs){
        const auto& data = EEC.result.get_data();

        nR.push_back(data.nR);
        nr.push_back(data.nr);
        nc.push_back(data.nc);

        iJet.push_back(EEC.iJet);
        iReco.push_back(EEC.iReco);
        pt_denom.push_back(EEC.result.get_pt_denom());

        if constexpr (ResultType::IS_ARRAY){
            int entries = 0;
            for (unsigned iR = 0; iR < data.get_data().shape()[0]; ++iR){
                for (unsigned ir = 0; ir < data.get_data().shape()[1]; ir++){
                    for (unsigned ic = 0; ic < data.get_data().shape()[2]; ic++){
                        if (data.get_data()[iR][ir][ic] > 0){
                            entries++;
                            R.push_back(iR);
                            r.push_back(ir);
                            c.push_back(ic);
                            wt.push_back(data.get_data()[iR][ir][ic]);
                        }
                    }
                }
            }
            if (entries == 0){
                nEntry.push_back(1);
                R.push_back(-1);
                r.push_back(-1);
                c.push_back(-1);
                wt.push_back(-1);
            } else {
                nEntry.push_back(entries);
            }
        } else {
            if (data.get_data().empty()){
                nEntry.push_back(1);
                R.push_back(-1);  
                r.push_back(-1);
                c.push_back(-1);
                wt.push_back(-1);
            } else {
                nEntry.push_back(data.get_data().size());
                for (const auto& [iR, ir, ic, iwt] : data.get_data()){
                    R.push_back(iR);
                    r.push_back(ir);
                    c.push_back(ic);
                    wt.push_back(iwt);
                }
            }
        }
    }

    auto dataTable = std::make_unique<nanoaod::FlatTable>(R.size(), name_, false);    
    dataTable->template addColumn<typename ResultType::T>("R", R, "R values", ResultType::COLUMN_TYPE);
    dataTable->template addColumn<typename ResultType::T>("r", r, "r values", ResultType::COLUMN_TYPE);
    dataTable->template addColumn<typename ResultType::T>("c", c, "c values", ResultType::COLUMN_TYPE);
    dataTable->template addColumn<float>("wt", wt, "wt values", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(dataTable), name_);

    auto BKTable = std::make_unique<nanoaod::FlatTable>(nR.size(), name_ + "BK", false);    
    BKTable->addColumn<int>("nR", nR, "nR", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("nr", nr, "nr", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("nc", nc, "nc", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("iJet", iJet, "iJet", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("iReco", iReco, "iReco", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<float>("pt_denom", pt_denom, "pt_denom", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<int>("nEntry", nEntry, "nEntry", nanoaod::FlatTable::IntColumn);
    event.put(std::move(BKTable), name_ + "BK");
}

typedef EECRes3TableProducer<EEC::CMSSWResult<EEC::Res3Result<EEC::ResVectorContainer<double>>>> EECRes3UnbinnedTableProducer;
typedef EECRes3TableProducer<EEC::CMSSWResult<EEC::Res3Result<EEC::ResVectorContainer<unsigned>>>> EECRes3VectorTableProducer;
typedef EECRes3TableProducer<EEC::CMSSWResult<EEC::Res3Result<EEC::ResMultiArrayContainer>>> EECRes3ArrayTableProducer;

DEFINE_FWK_MODULE(EECRes3UnbinnedTableProducer);
DEFINE_FWK_MODULE(EECRes3VectorTableProducer);
DEFINE_FWK_MODULE(EECRes3ArrayTableProducer);

