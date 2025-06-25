import FWCore.ParameterSet.Config as cms

EECRes3MatchedVectorProducer = cms.EDProducer("EECRes3MatchedVectorProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECRes3MatchedUnbinnedProducer = cms.EDProducer("EECRes3MatchedUnbinnedProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECRes3MatchedArrayProducer = cms.EDProducer("EECRes3MatchedArrayProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

