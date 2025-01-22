#include "FWCore/Framework/interface/MakerMacros.h"
#include "CommonTools/UtilAlgos/interface/ObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/SortCollectionSelector.h"
#include "CommonTools/Utils/interface/PtComparator.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/JetCollection.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

typedef ObjectSelector<SortCollectionSelector<reco::JetView, GreaterByPt<reco::Jet> > >
    LeadingJetSelector;
typedef ObjectSelector<SortCollectionSelector<reco::GenJetCollection, GreaterByPt<reco::GenJet> > >
    LeadingGenJetSelector;
typedef ObjectSelector<SortCollectionSelector<pat::JetCollection, GreaterByPt<pat::Jet> > >
    LeadingPatJetSelector;

DEFINE_FWK_MODULE(LeadingJetSelector);
DEFINE_FWK_MODULE(LeadingGenJetSelector);
DEFINE_FWK_MODULE(LeadingPatJetSelector);
