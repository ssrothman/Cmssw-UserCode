import FWCore.ParameterSet.Config as cms

EECProjVectorTableProducer = cms.EDProducer("EECProjVectorTableProducer",
    name = cms.string(""),
    EECs = cms.InputTag("")
)

EECProjUnbinnedTableProducer = cms.EDProducer("EECProjUnbinnedTableProducer",
    name = cms.string(""),
    EECs = cms.InputTag("")
)

EECProjArrayTableProducer = cms.EDProducer("EECProjArrayTableProducer",
    name = cms.string(""),
    EECs = cms.InputTag("")
)

