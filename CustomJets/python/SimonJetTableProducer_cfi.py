import FWCore.ParameterSet.Config as cms

SimonJetTableProducer = cms.EDProducer("SimonJetTableProducer",
    src = cms.InputTag("PatSimonJetProducer"),
    name = cms.string("SimonJets"),
    addMatch = cms.bool(False),
    isGen = cms.bool(False),
    matchSrc = cms.InputTag(""),
    otherMatchSrc = cms.InputTag(""),
    doOtherMatch = cms.bool(False),
    verbose = cms.int32(1),
    genJets = cms.InputTag("")
)
