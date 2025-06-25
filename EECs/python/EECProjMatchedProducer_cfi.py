import FWCore.ParameterSet.Config as cms

EECProjMatchedVectorProducer = cms.EDProducer("EECProjMatchedVectorProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECProjMatchedUnbinnedProducer = cms.EDProducer("EECProjMatchedUnbinnedProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECProjMatchedArrayProducer = cms.EDProducer("EECProjMatchedArrayProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

