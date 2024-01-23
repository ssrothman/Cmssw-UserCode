#include "CommonTools/UtilAlgos/interface/SingleObjectSelector.h"
#include "CommonTools/UtilAlgos/interface/StringCutObjectSelector.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

namespace pat{
    typedef SingleObjectSelector<
        std::vector<PackedCandidate>,
        StringCutObjectSelector<PackedCandidate, true>
    > PATPackedCandidateSelector;
};

using namespace pat;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATPackedCandidateSelector );
