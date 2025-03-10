import FWCore.ParameterSet.Config as cms

from res4calculator_cfi import res4calculator

EECRes4Producer = cms.EDProducer("EECRes4Producer",
    jets = cms.InputTag(""),
    calculator = res4calculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)
