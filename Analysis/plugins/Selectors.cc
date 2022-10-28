#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "CommonTools/UtilAlgos/interface/ObjectCountFilter.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "CommonTools/UtilAlgos/interface/ParameterAdapter.h"
#include "CommonTools/Utils/interface/FunctionMinSelector.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "CommonTools/UtilAlgos/interface/ParameterAdapter.h"
#include "CommonTools/Utils/interface/FunctionMinSelector.h"
#include "CommonTools/RecoAlgos/plugins/SuperClusterEt.h"


//typedef SingleObjectSelector<edm::View<pat::Muon>, StringCutObjectSelector<pat::Muon>, pat::MuonCollection> PatMuonSelector;

//typedef ObjectCountFilter<pat::Muon, PatMuonSelector>::type PtMuonCountFilter;

//DEFINE_FWK_MODULE(PtMuonCountFilter);

typedef ObjectCountFilter<pat::MuonCollection>::type MuonCountFilter;

DEFINE_FWK_MODULE(MuonCountFilter);
