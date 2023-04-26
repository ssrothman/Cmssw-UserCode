import FWCore.ParameterSet.Config as cms

RecoEECProducer = cms.EDProducer("RecoEECProducer",
    reco = cms.InputTag("SimonJets"),
    maxOrder = cms.uint32(6),
    verbose = cms.int32(0)
)
