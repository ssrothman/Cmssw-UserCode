import FWCore.ParameterSet.Config as cms

EECTransferTableProducer = cms.EDProducer("EECTransferTableProducer",
    src = cms.InputTag("EECProducer:transfer"),
    name = cms.string("EECTransfer"),
    verbose = cms.int32(1),
    orders = cms.vint32(2,3,21)
)

