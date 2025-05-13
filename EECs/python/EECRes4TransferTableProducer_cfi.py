import FWCore.ParameterSet.Config as cms

EECRes4TransferVectorTableProducer = cms.EDProducer("EECRes4TransferVectorTableProducer",
    name = cms.string(""),
    EECTransfer = cms.InputTag("")
)

EECRes4TransferUnbinnedTableProducer = cms.EDProducer("EECRes4TransferUnbinnedTableProducer",
    name = cms.string(""),
    EECTransfer = cms.InputTag("")
)

EECRes4TransferArrayTableProducer = cms.EDProducer("EECRes4TransferArrayTableProducer",
    name = cms.string(""),
    EECTransfer = cms.InputTag("")
)
