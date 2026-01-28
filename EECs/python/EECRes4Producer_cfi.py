import FWCore.ParameterSet.Config as cms

EECRes4VectorProducer = cms.EDProducer("EECRes4VectorProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

EECRes4UnbinnedProducer = cms.EDProducer("EECRes4UnbinnedProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

EECRes4ArrayProducer = cms.EDProducer("EECRes4ArrayProducer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)
