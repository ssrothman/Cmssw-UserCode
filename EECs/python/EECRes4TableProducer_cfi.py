import FWCore.ParameterSet.Config as cms

EECRes4TableProducer = cms.EDProducer("EECRes4TableProducer",
    name = cms.string(""),
    EECs = cms.InputTag("")
)
