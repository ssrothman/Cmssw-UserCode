import FWCore.ParameterSet.Config as cms

EECProjMatchedVectorProducer = cms.EDProducer("EECProjMatchedVectorProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),

    checkGenFlags = cms.bool(False),
    genJets = cms.InputTag(""),
    genFlags = cms.VInputTag(),
)

EECProjMatchedUnbinnedProducer = cms.EDProducer("EECProjMatchedUnbinnedProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),

    checkGenFlags = cms.bool(False),
    genJets = cms.InputTag(""),
    genFlags = cms.VInputTag(),
)

EECProjMatchedArrayProducer = cms.EDProducer("EECProjMatchedArrayProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),

    checkGenFlags = cms.bool(False),
    genJets = cms.InputTag(""),
    genFlags = cms.VInputTag(),
)

