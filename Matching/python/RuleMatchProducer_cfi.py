import FWCore.ParameterSet.Config as cms

RuleMatchProducer = cms.EDProducer('RuleMatchProducer',
    jetMatchingDR = cms.double(0.2), 
    partMatchingDR = cms.double(0.05),
    pTtoleranceEM0 = cms.double(-1),
    pTtoleranceHAD0 = cms.double(-1),
    pTtoleranceCH = cms.double(-1),
    allowManyEM0 = cms.bool(True),
    allowManyHAD0 = cms.bool(True),
    allowManyCH = cms.bool(False),
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("GenSimonJets"),
    verbose = cms.int32(0)
)
