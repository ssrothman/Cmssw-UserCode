import FWCore.ParameterSet.Config as cms

EECProjMatchedVectorProducer = cms.EDProducer("EECProjMatchedVectorProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

EECProjMatchedUnbinnedProducer = cms.EDProducer("EECProjMatchedUnbinnedProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

EECProjMatchedArrayProducer = cms.EDProducer("EECProjMatchedArrayProducer",
    jets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(),
    flags = cms.VInputTag(),
)

