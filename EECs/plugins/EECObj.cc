#include "SRothman/EECs/plugins/EECObj.h"
#include "PhysicsTools/NanoAOD/interface/SimpleFlatTableProducer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef SimpleFlatTableProducer<EECObj> SimpleEECObjFlatTableProducer;

DEFINE_FWK_MODULE(SimpleEECObjFlatTableProducer);
