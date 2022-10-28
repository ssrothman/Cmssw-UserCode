#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "CommonTools/UtilAlgos/interface/ObjectCountFilter.h"

//typedef SingleObjectSelector<edm::View<pat::Muon>, StringCutObjectSelector<pat::Muon>, pat::MuonCollection> PatMuonSelector;

//typedef ObjectCountFilter<pat::Muon, PatMuonSelector>::type PtMuonCountFilter;

//DEFINE_FWK_MODULE(PtMuonCountFilter);


//typedef SingleObjectSelector<edm::View<pat::Muon>, StringCutObjectSelector<pat::Muon>, pat::MuonCollection> MuonSelector;


//DEFINE_FWK_MODULE(MuonSelector);
