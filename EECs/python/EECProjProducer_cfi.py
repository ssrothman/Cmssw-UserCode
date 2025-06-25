import FWCore.ParameterSet.Config as cms

EECProjVectorProducer = cms.EDProducer("EECProjVectorProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECProjUnbinnedProducer = cms.EDProducer("EECProjUnbinnedProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECProjArrayProducer = cms.EDProducer("EECProjArrayProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

