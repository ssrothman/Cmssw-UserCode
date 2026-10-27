import FWCore.ParameterSet.Config as cms

EECRes4TransferVectorProducer = cms.EDProducer("EECRes4TransferVectorProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),

    calculator = cms.PSet(),
    basicCalculator = cms.PSet(),

    recoFlags = cms.VInputTag(),
    genFlags = cms.VInputTag(),
)

EECRes4TransferUnbinnedProducer = cms.EDProducer("EECRes4TransferUnbinnedProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),

    calculator = cms.PSet(),
    basicCalculator = cms.PSet(),

    recoFlags = cms.VInputTag(),
    genFlags = cms.VInputTag(),
)

EECRes4TransferArrayProducer = cms.EDProducer("EECRes4TransferArrayProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),

    calculator = cms.PSet(),
    basicCalculator = cms.PSet(),

    recoFlags = cms.VInputTag(),
    genFlags = cms.VInputTag(),
)
