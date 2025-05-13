import FWCore.ParameterSet.Config as cms

from projcalculator_cfi import projcalculator

EECProjVectorProducer = cms.EDProducer("EECProjVectorProducer",
    jets = cms.InputTag(""),
    calculator = projcalculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECProjUnbinnedProducer = cms.EDProducer("EECProjUnbinnedProducer",
    jets = cms.InputTag(""),
    calculator = projcalculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

EECProjArrayProducer = cms.EDProducer("EECProjArrayProducer",
    jets = cms.InputTag(""),
    calculator = projcalculator,
    flags = cms.VInputTag("ChargedSimonJetsPreselection",
                          "ChargedSimonJetsOverlapVeto"),
)

