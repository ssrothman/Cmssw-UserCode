import FWCore.ParameterSet.Config as cms

EECTableProducer = cms.EDProducer("EECTableProducer",
    src = cms.InputTag("EECProducer"),
    name = cms.string("EEC"),
    verbose = cms.int32(1),
    orders = cms.vint32(2,3,21)
)
