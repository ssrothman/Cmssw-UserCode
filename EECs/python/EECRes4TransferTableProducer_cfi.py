import FWCore.ParameterSet.Config as cms

EECRes4TransferTableProducer = cms.EDProducer("EECRes4TransferTableProducer",
    name = cms.string(""),
    EECTransfer = cms.InputTag("")
)
