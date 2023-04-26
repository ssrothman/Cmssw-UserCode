import FWCore.ParameterSet.Config as cms

PatSimonJetProducer = cms.EDProducer("PatSimonJetProducer",
    src = cms.InputTag("selectedPatJets"),
    minPartPt = cms.double(-1.0),
    verbose = cms.int32(0),
    doUncertainty = cms.bool(True),
    minPt = cms.double(30),
    maxEta = cms.double(2.5),
    maxMuFrac = cms.double(0.8),
    maxChEmFrac = cms.double(0.8)
)

GenSimonJetProducer = cms.EDProducer("GenSimonJetProducer",
    src = cms.InputTag("ak4GenJetsNoNu"),
    minPartPt = cms.double(-1.0),
    verbose = cms.int32(0),
    doUncertainty = cms.bool(False),
    minPt = cms.double(20),
    maxEta = cms.double(3.0),
    maxMuFrac = cms.double(0.8),
    maxChEmFrac = cms.double(0.8)
)
