import FWCore.ParameterSet.Config as cms

CorrectedMuonProducer = cms.EDProducer('CorrectedMuonProducer',
    src = cms.InputTag('linkedObjects','muons'),
    RoccoR = cms.InputTag("RoccoR"),
    verbose = cms.int32(0),
)

