import FWCore.ParameterSet.Config as cms

EECTableProducer = cms.EDProducer("EECTableProducer",
    src = cms.InputTag("EECProducer"),
    name = cms.string("EEC")
)
