import FWCore.ParameterSet.Config as cms

from SRothman.EECs.EECTableProducer_cfi import EECTableProducer

EECTransferTableProducer = cms.EDProducer("EECTransferTableProducer",
    src = cms.InputTag("EECProducer:transfer"),
    name = cms.string("EECTransfer"),
    verbose = cms.int32(1),
    orders = EECTableProducer.orders
)

