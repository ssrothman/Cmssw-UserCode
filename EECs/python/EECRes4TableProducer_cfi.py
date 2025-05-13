import FWCore.ParameterSet.Config as cms

EECRes4VectorTableProducer = cms.EDProducer("EECRes4VectorTableProducer",
    name = cms.string(""),
    EECs = cms.InputTag("")
)

EECRes4UnbinnedTableProducer = cms.EDProducer("EECRes4UnbinnedTableProducer",
    name = cms.string(""),
    EECs = cms.InputTag("")
)

EECRes4ArrayTableProducer = cms.EDProducer("EECRes4ArrayTableProducer",
    name = cms.string(""),
    EECs = cms.InputTag("")
)
