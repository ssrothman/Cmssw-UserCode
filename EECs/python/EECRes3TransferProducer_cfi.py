import FWCore.ParameterSet.Config as cms

EECRes3TransferVectorProducer = cms.EDProducer("EECRes3TransferVectorProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

EECRes3TransferUnbinnedProducer = cms.EDProducer("EECRes3TransferUnbinnedProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

EECRes3TransferArrayProducer = cms.EDProducer("EECRes3TransferArrayProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

