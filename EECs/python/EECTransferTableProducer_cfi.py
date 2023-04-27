import FWCore.ParameterSet.Config as cms

EECTransferTableProducer = cms.EDProducer("EECTransferTableProducer",
    src = cms.InputTag("EECProducer:transfer"),
    name = cms.string("EECTransfer")
)

