import FWCore.ParameterSet.Config as cms

EECProjVectorProducer = cms.EDProducer("EECProjVectorProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),

    checkGenFlags = cms.bool(False),
    genJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    genFlags = cms.VInputTag(),
)

EECProjUnbinnedProducer = cms.EDProducer("EECProjUnbinnedProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),

    checkGenFlags = cms.bool(False),
    genJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    genFlags = cms.VInputTag(),
)

EECProjArrayProducer = cms.EDProducer("EECProjArrayProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),

    checkGenFlags = cms.bool(False),
    genJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    genFlags = cms.VInputTag(),
)

