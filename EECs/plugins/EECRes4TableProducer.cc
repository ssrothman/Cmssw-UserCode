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

    std::vector<float> dipole_R, dipole_r, dipole_c;
    std::vector<float> dipole_wt;

    std::vector<float> tee_R, tee_r, tee_c;
    std::vector<float> tee_wt;

    std::vector<float> triangle_R, triangle_r, triangle_c;
    std::vector<float> triangle_wt;

    std::vector<float> nR_dipole, nr_dipole, nc_dipole;
    std::vector<float> nR_tee, nr_tee, nc_tee;
    std::vector<float> nR_triangle, nr_triangle, nc_triangle;
    std::vector<float> nEntry_dipole, nEntry_tee, nEntry_triangle;

    std::vector<float> iJet, iReco;
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

        if (dipole.get_data().empty()){
            nEntry_dipole.push_back(1);
            dipole_R.push_back(-1);  
            dipole_r.push_back(-1);
            dipole_c.push_back(-1);
            dipole_wt.push_back(-1);
        } else {
            nEntry_dipole.push_back(dipole.get_data().size());
            for (const auto& [R, r, c, wt] : dipole.get_data()){
                dipole_R.push_back(R);
                dipole_r.push_back(r);
                dipole_c.push_back(c);
                dipole_wt.push_back(wt);
            }
        }

        if (tee.get_data().empty()){
            nEntry_tee.push_back(1);
            tee_R.push_back(-1);
            tee_r.push_back(-1);
            tee_c.push_back(-1);
            tee_wt.push_back(-1);
        } else {
            nEntry_tee.push_back(tee.get_data().size());
            for (const auto& [R, r, c, wt] : tee.get_data()){
                tee_R.push_back(R);
                tee_r.push_back(r);
                tee_c.push_back(c);
                tee_wt.push_back(wt);
            }
        }

        if (triangle.get_data().empty()){
            nEntry_triangle.push_back(1);
            triangle_R.push_back(-1);
            triangle_r.push_back(-1);
            triangle_c.push_back(-1);
            triangle_wt.push_back(-1);
        } else {
            nEntry_triangle.push_back(triangle.get_data().size());
            for (const auto& [R, r, c, wt] : triangle.get_data()){
                triangle_R.push_back(R);
                triangle_r.push_back(r);
                triangle_c.push_back(c);
                triangle_wt.push_back(wt);
            }
        }
    }

    auto dipoleTable = std::make_unique<nanoaod::FlatTable>(dipole_R.size(), name_ + "dipole", false);
    dipoleTable->addColumn<float>("R", dipole_R, "R values", nanoaod::FlatTable::FloatColumn);
    dipoleTable->addColumn<float>("r", dipole_r, "r values", nanoaod::FlatTable::FloatColumn);
    dipoleTable->addColumn<float>("c", dipole_c, "c values", nanoaod::FlatTable::FloatColumn);
    dipoleTable->addColumn<float>("wt", dipole_wt, "wt values", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(dipoleTable), name_ + "dipole");

    auto teeTable = std::make_unique<nanoaod::FlatTable>(tee_R.size(), name_ + "tee", false);
    teeTable->addColumn<float>("R", tee_R, "R values", nanoaod::FlatTable::FloatColumn);
    teeTable->addColumn<float>("r", tee_r, "r values", nanoaod::FlatTable::FloatColumn);
    teeTable->addColumn<float>("c", tee_c, "c values", nanoaod::FlatTable::FloatColumn);
    teeTable->addColumn<float>("wt", tee_wt, "wt values", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(teeTable), name_ + "tee");

    auto triangleTable = std::make_unique<nanoaod::FlatTable>(triangle_R.size(), name_ + "triangle", false);    
    triangleTable->addColumn<float>("R", triangle_R, "R values", nanoaod::FlatTable::FloatColumn);
    triangleTable->addColumn<float>("r", triangle_r, "r values", nanoaod::FlatTable::FloatColumn);
    triangleTable->addColumn<float>("c", triangle_c, "c values", nanoaod::FlatTable::FloatColumn);
    triangleTable->addColumn<float>("wt", triangle_wt, "wt values", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(triangleTable), name_ + "triangle");

    auto BKTable = std::make_unique<nanoaod::FlatTable>(EECs->size(), name_ + "BK", false);
    BKTable->addColumn<float>("nR_dipole", nR_dipole, "nR dipole", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nr_dipole", nr_dipole, "nr dipole", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nc_dipole", nc_dipole, "nc dipole", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nR_tee", nR_tee, "nR tee", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nr_tee", nr_tee, "nr tee", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nc_tee", nc_tee, "nc tee", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nR_triangle", nR_triangle, "nR triangle", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nr_triangle", nr_triangle, "nr triangle", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nc_triangle", nc_triangle, "nc triangle", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("iJet", iJet, "iJet", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("iReco", iReco, "iReco", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("pt_denom", pt_denom, "pt_denom", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nEntry_dipole", nEntry_dipole, "nEntry dipole", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nEntry_tee", nEntry_tee, "nEntry tee", nanoaod::FlatTable::FloatColumn);
    BKTable->addColumn<float>("nEntry_triangle", nEntry_triangle, "nEntry triangle", nanoaod::FlatTable::FloatColumn);
    event.put(std::move(BKTable), name_ + "BK");
}

DEFINE_FWK_MODULE(EECRes4TableProducer);
