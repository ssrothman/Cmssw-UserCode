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

class EECRes4TableProducer : public edm::stream::EDProducer<> {
public:
    explicit EECRes4TableProducer(const edm::ParameterSet&);
    ~EECRes4TableProducer() override = default;
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
    void produce(edm::Event&, const edm::EventSetup&) override;
private:
    std::string name_;

    edm::EDGetTokenT<std::vector<EEC::CMSSWRes4Result>> EECToken_;
};

EECRes4TableProducer::EECRes4TableProducer(const edm::ParameterSet& conf) :
        name_(conf.getParameter<std::string>("name")),
        EECToken_(consumes<std::vector<EEC::CMSSWRes4Result>>(conf.getParameter<edm::InputTag>("EECs"))) {

    produces<nanoaod::FlatTable>(name_+"dipole");
    produces<nanoaod::FlatTable>(name_+"tee");
    produces<nanoaod::FlatTable>(name_+"triangle");
    produces<nanoaod::FlatTable>(name_+"BK");
}

void EECRes4TableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    desc.add<std::string>("name");
    desc.add<edm::InputTag>("EECs");
    descriptions.add("EECRes4TableProducer", desc);
}

void EECRes4TableProducer::produce(edm::Event& event, const edm::EventSetup& setup){
    edm::Handle<std::vector<EEC::CMSSWRes4Result>> EECs;
    event.getByToken(EECToken_, EECs);

    std::vector<float> dipole_flat;
    std::vector<float> tee_flat;
    std::vector<float> triangle_flat;

    std::vector<int> nR_dipole, nr_dipole, nc_dipole;
    std::vector<int> nR_tee, nr_tee, nc_tee;
    std::vector<int> nR_triangle, nr_triangle, nc_triangle;

    std::vector<int> iJet, iReco;
    std::vector<float> pt_denom;

    for (const auto& EEC : *EECs){
        const auto& dipole = EEC.result.get_dipole();
        const auto& tee = EEC.result.get_tee();
        const auto& triangle = EEC.result.get_triangle();

        nR_dipole.push_back(dipole.nR);
        nr_dipole.push_back(dipole.nr);
        nc_dipole.push_back(dipole.nc);

        nR_tee.push_back(tee.nR);
        nr_tee.push_back(tee.nr);
        nc_tee.push_back(tee.nc);

        nR_triangle.push_back(triangle.nR);
        nr_triangle.push_back(triangle.nr);
        nc_triangle.push_back(triangle.nc);

        iJet.push_back(EEC.iJet);
        iReco.push_back(EEC.iReco);
        pt_denom.push_back(EEC.result.get_pt_denom());

        dipole_flat.insert(
                dipole_flat.end(), 
                dipole.get_data().data(),
                dipole.get_data().data() + dipole.get_data().num_elements());

        tee_flat.insert(
                tee_flat.end(), 
                tee.get_data().data(),
                tee.get_data().data() + tee.get_data().num_elements());

        triangle_flat.insert(
                triangle_flat.end(), 
                triangle.get_data().data(),
                triangle.get_data().data() + triangle.get_data().num_elements());
    }

    auto dipoleTable = std::make_unique<nanoaod::FlatTable>(dipole_flat.size(), name_ + "dipole", false);
    dipoleTable->addColumn<float>("value", dipole_flat, "dipole values", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(dipoleTable), name_ + "dipole");

    auto teeTable = std::make_unique<nanoaod::FlatTable>(tee_flat.size(), name_ + "tee", false);
    teeTable->addColumn<float>("value", tee_flat, "tee values", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(teeTable), name_ + "tee");

    auto triangleTable = std::make_unique<nanoaod::FlatTable>(triangle_flat.size(), name_ + "triangle", false);    
    triangleTable->addColumn<float>("value", triangle_flat, "triangle values", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(triangleTable), name_ + "triangle");

    auto BKTable = std::make_unique<nanoaod::FlatTable>(EECs->size(), name_ + "BK", false);
    BKTable->addColumn<int>("nR_dipole", nR_dipole, "nR dipole", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("nr_dipole", nr_dipole, "nr dipole", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("nc_dipole", nc_dipole, "nc dipole", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("nR_tee", nR_tee, "nR tee", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("nr_tee", nr_tee, "nr tee", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("nc_tee", nc_tee, "nc tee", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("nR_triangle", nR_triangle, "nR triangle", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("nr_triangle", nr_triangle, "nr triangle", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("nc_triangle", nc_triangle, "nc triangle", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("iJet", iJet, "iJet", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<int>("iReco", iReco, "iReco", nanoaod::FlatTable::IntColumn);
    BKTable->addColumn<float>("pt_denom", pt_denom, "pt_denom", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(BKTable), name_ + "BK");
}

DEFINE_FWK_MODULE(EECRes4TableProducer);
