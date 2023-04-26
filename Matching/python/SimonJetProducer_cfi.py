import FWCore.ParameterSet.Config as cms

PatSimonJetProducer = cms.EDProducer("PatSimonJetProducer",
    src = cms.InputTag("selectedPatJets"),
    minPartPt = cms.double(-1.0),
    verbose = cms.int32(0)
)

GenSimonJetProducer = cms.EDProducer("GenSimonJetProducer",
    src = cms.InputTag("ak4GenJetsNoNu"),
    minPartPt = cms.double(-1.0),
    verbose = cms.int32(0)
)
