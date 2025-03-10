import FWCore.ParameterSet.Config as cms

from res4calculator_cfi import res4calculator

EECRes4MatchedProducer = cms.EDProducer("EECRes4MatchedProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = res4calculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)
