import FWCore.ParameterSet.Config as cms

ZMuMuEventSelectionProducer = cms.EDProducer('ZMuMuEventSelectionProducer',
    src = cms.InputTag('muons'),
    leadMuPt = cms.double(15.0),
    subMuPt = cms.double(8.0),
    maxMuEta = cms.double(3.0),
    ID = cms.string("loose"),
    Iso = cms.string("loose"),
    minZmass = cms.double(50.0),
    maxZmass = cms.double(130.0),
    verbose = cms.int32(1)
)
