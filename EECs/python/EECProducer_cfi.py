import FWCore.ParameterSet.Config as cms

EECProducer = cms.EDProducer("EECProducer",
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("GenSimonJets"),
    match = cms.InputTag("GenMatch"),
    doGen = cms.bool(True),

    maxOrder = cms.uint32(6),
    doRes3 = cms.bool(True),
    doRes4 = cms.bool(True),
    p1s = cms.vuint32(2, 2, 3),
    p2s = cms.vuint32(1, 2, 1),

    verbose = cms.int32(1),
)
