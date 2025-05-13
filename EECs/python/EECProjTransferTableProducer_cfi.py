import FWCore.ParameterSet.Config as cms

EECProjTransferVectorTableProducer = cms.EDProducer("EECProjTransferVectorTableProducer",
    name = cms.string(""),
    EECTransfer = cms.InputTag("")
)

EECProjTransferUnbinnedTableProducer = cms.EDProducer("EECProjTransferUnbinnedTableProducer",
    name = cms.string(""),
    EECTransfer = cms.InputTag("")
)

EECProjTransferArrayTableProducer = cms.EDProducer("EECProjTransferArrayTableProducer",
    name = cms.string(""),
    EECTransfer = cms.InputTag("")
)

