import FWCore.ParameterSet.Config as cms

EECRes3VectorProducer = cms.EDProducer("EECRes3VectorProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECRes3UnbinnedProducer = cms.EDProducer("EECRes3UnbinnedProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECRes3ArrayProducer = cms.EDProducer("EECRes3ArrayProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

