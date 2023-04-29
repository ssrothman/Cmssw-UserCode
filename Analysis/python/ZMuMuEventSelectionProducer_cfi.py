import FWCore.ParameterSet.Config as cms

ZMuMuEventSelectionProducer = cms.EDProducer('ZMuMuEventSelectionProducer',
    src = cms.InputTag('muons'),
    minMuPt = cms.double(15.0),
    maxMuEta = cms.double(2.4),
    ID = cms.string("tight"),
    Iso = cms.string("tight"),
    minZmass = cms.double(60.0),
    maxZmass = cms.double(120.0),
    verbose = cms.int32(1)
)