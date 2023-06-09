import FWCore.ParameterSet.Config as cms

ZMuMuEventSelectionProducer = cms.EDProducer('ZMuMuEventSelectionProducer',
    src = cms.InputTag('muons'),
    leadMuPt = cms.double(20.0),
    subMuPt = cms.double(10.0),
    maxMuEta = cms.double(2.4),
    ID = cms.string("medium"),
    Iso = cms.string("medium"),
    minZmass = cms.double(60.0),
    maxZmass = cms.double(120.0),
    verbose = cms.int32(1)
)
