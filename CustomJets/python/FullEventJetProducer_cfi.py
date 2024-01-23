import FWCore.ParameterSet.Config as cms

FullEventJetProducer = cms.EDProducer("FullEventJetProducer",
    minPartPt = cms.double(1e-3),
    onlyFromPV = cms.bool(False),
    onlyCharged = cms.bool(False),
    maxNumPart = cms.uint32(2048),
    maxEta = cms.double(2.4),
    applyPuppi = cms.bool(True),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("pupppi"),

    verbose = cms.int32(0)
)
