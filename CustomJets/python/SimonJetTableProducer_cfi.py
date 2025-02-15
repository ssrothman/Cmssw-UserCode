import FWCore.ParameterSet.Config as cms

SimonJetTableProducer = cms.EDProducer("SimonJetTableProducer",
    src = cms.InputTag("PatSimonJetProducer"),
    name = cms.string("SimonJets"),
    verbose = cms.int32(1),
)
