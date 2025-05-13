import FWCore.ParameterSet.Config as cms

from res3calculator_cfi import res3calculator

EECRes3VectorProducer = cms.EDProducer("EECRes3VectorProducer",
    jets = cms.InputTag(""),
    calculator = res3calculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECRes3UnbinnedProducer = cms.EDProducer("EECRes3UnbinnedProducer",
    jets = cms.InputTag(""),
    calculator = res3calculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECRes3ArrayProducer = cms.EDProducer("EECRes3ArrayProducer",
    jets = cms.InputTag(""),
    calculator = res3calculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

