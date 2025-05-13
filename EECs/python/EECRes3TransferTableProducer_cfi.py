import FWCore.ParameterSet.Config as cms

EECRes3TransferVectorTableProducer = cms.EDProducer("EECRes3TransferVectorTableProducer",
    name = cms.string(""),
    EECTransfer = cms.InputTag("")
)

EECRes3TransferUnbinnedTableProducer = cms.EDProducer("EECRes3TransferUnbinnedTableProducer",
    name = cms.string(""),
    EECTransfer = cms.InputTag("")
)

EECRes3TransferArrayTableProducer = cms.EDProducer("EECRes3TransferArrayTableProducer",
    name = cms.string(""),
    EECTransfer = cms.InputTag("")
)

