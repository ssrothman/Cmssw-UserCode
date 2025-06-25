import FWCore.ParameterSet.Config as cms

EECRes4MatchedVectorProducer = cms.EDProducer("EECRes4MatchedVectorProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECRes4MatchedUnbinnedProducer = cms.EDProducer("EECRes4MatchedUnbinnedProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECRes4MatchedArrayProducer = cms.EDProducer("EECRes4MatchedArrayProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)
