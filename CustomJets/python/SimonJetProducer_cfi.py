import FWCore.ParameterSet.Config as cms

PatSimonJetProducer = cms.EDProducer("PatSimonJetProducer",
    selector = cms.PSet(
        parameters = cms.PSet(),
        settings = cms.PSet(),
    ),

    jetSrc = cms.InputTag(""),

    CHSsrc = cms.InputTag(""),

    addCHSindex = cms.bool(False),
    CHSmatchDR = cms.double(-1),

    verbose = cms.int32(0),
)

GenSimonJetProducer = cms.EDProducer("GenSimonJetProducer",
    selector = cms.PSet(
        parameters = cms.PSet(),
        settings = cms.PSet(),
    ),

    jetSrc = cms.InputTag(""),

    CHSsrc = cms.InputTag(""),
    addCHSindex = cms.bool(False),
    CHSmatchDR = cms.double(-1),

    verbose = cms.int32(0),
)