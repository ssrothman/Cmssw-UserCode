import FWCore.ParameterSet.Config as cms

EECRes4MatchedVectorProducer = cms.EDProducer("EECRes4MatchedVectorProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),

    checkGenFlags = cms.bool(False),
    genJets = cms.InputTag(""),
    genFlags = cms.VInputTag(),
)

EECRes4MatchedUnbinnedProducer = cms.EDProducer("EECRes4MatchedUnbinnedProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),

    checkGenFlags = cms.bool(False),
    genJets = cms.InputTag(""),
    genFlags = cms.VInputTag(),
)

EECRes4MatchedArrayProducer = cms.EDProducer("EECRes4MatchedArrayProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),

    checkGenFlags = cms.bool(False),
    genJets = cms.InputTag(""),
    genFlags = cms.VInputTag(),
)
