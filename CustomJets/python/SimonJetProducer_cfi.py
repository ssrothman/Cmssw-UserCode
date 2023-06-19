import FWCore.ParameterSet.Config as cms

PatSimonJetProducer = cms.EDProducer("PatSimonJetProducer",
    src = cms.InputTag("selectedPatJets"),
    minPartPt = cms.double(-1.0),
    maxNumPart = cms.uint32(32),
    minNumPart = cms.uint32(2),
    verbose = cms.int32(1),
    minPt = cms.double(30),
    maxEta = cms.double(2.5),
    maxMuFrac = cms.double(0.8),
    maxChEmFrac = cms.double(0.8),
    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),
)

GenSimonJetProducer = cms.EDProducer("GenSimonJetProducer",
    src = cms.InputTag("ak4GenJetsNoNu"),
    minPartPt = cms.double(-1.0),
    maxNumPart = cms.uint32(32),
    minNumPart = cms.uint32(2),
    verbose = cms.int32(1),
    minPt = cms.double(20),
    maxEta = cms.double(3.0),
    maxMuFrac = cms.double(0.8),
    maxChEmFrac = cms.double(0.8),
    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),
)
