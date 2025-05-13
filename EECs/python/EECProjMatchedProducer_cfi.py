import FWCore.ParameterSet.Config as cms

from projcalculator_cfi import projcalculator

EECProjMatchedVectorProducer = cms.EDProducer("EECProjMatchedVectorProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = projcalculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECProjMatchedUnbinnedProducer = cms.EDProducer("EECProjMatchedUnbinnedProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = projcalculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECProjMatchedArrayProducer = cms.EDProducer("EECProjMatchedArrayProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = projcalculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

