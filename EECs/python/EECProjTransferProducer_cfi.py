import FWCore.ParameterSet.Config as cms

EECProjTransferVectorProducer = cms.EDProducer("EECProjTransferVectorProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

EECProjTransferUnbinnedProducer = cms.EDProducer("EECProjTransferUnbinnedProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

EECProjTransferArrayProducer = cms.EDProducer("EECProjTransferArrayProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

