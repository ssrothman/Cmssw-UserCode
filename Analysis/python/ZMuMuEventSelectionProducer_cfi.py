import FWCore.ParameterSet.Config as cms

ZMuMuEventSelectionProducer = cms.EDProducer('ZMuMuEventSelectionProducer',
    src = cms.InputTag('muons'),
    leadMuPt = cms.double(23.0),
    subMuPt = cms.double(10.0),
    maxMuEta = cms.double(3.0),
    ID = cms.string("medium"),
    Iso = cms.string("loose"),
    minZmass = cms.double(65.0),
    maxZmass = cms.double(115.0),
    verbose = cms.int32(0)
)
