#ifndef SROTHMAN_EECS_EECOBJ
#define SROTHMAN_EECS_EECOBJ

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronCore.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronCoreFwd.h"
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/PatCandidates/interface/Lepton.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Common/interface/AtomicPtrCache.h"

#include <vector>

class EECObj{
    public:
        explicit EECObj(std::vector<float> dRs, std::vector<float> wts, std::vector<unsigned> nDRs){
            dRs_ = dRs;
            wts_ = wts;
            nDRs_ = nDRs;
        }

        std::vector<float> dRs(){
            return dRs_;
        }

        std::vector<float> wts(){
            return wts_;
        }

        std::vector<unsigned> nDRs(){
            return nDRs_;
        }

    private:
        std::vector<float> dRs_, wts_;
        std::vector<unsigned> nDRs_;
};

typedef std::vector<EECObj> EECObjCollection;
typedef edm::Ref<EECObjCollection> EECObjRef;
typedef edm::RefVector<EECObjCollection> EECObjRefVector;

#endif