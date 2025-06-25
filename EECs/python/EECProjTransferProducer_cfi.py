import FWCore.ParameterSet.Config as cms

EECProjTransferVectorProducer = cms.EDProducer("EECProjTransferVectorProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECProjTransferUnbinnedProducer = cms.EDProducer("EECProjTransferUnbinnedProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECProjTransferArrayProducer = cms.EDProducer("EECProjTransferArrayProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

