import FWCore.ParameterSet.Config as cms

EECRes3VectorTableProducer = cms.EDProducer("EECRes3VectorTableProducer",
    name = cms.string(""),
    EECs = cms.InputTag("")
)

EECRes3UnbinnedTableProducer = cms.EDProducer("EECRes3UnbinnedTableProducer",
    name = cms.string(""),
    EECs = cms.InputTag("")
)

EECRes3ArrayTableProducer = cms.EDProducer("EECRes3ArrayTableProducer",
    name = cms.string(""),
    EECs = cms.InputTag("")
)

