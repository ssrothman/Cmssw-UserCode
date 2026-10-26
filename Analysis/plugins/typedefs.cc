#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"

typedef SingleObjectSelector<
    std::vector<pat::PackedGenParticle>,
    StringCutObjectSelector<pat::PackedGenParticle, true>  // true => lazy parsing => get all methods of daughters
>
PATPackedGenParticleSelector;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PATPackedGenParticleSelector);