import FWCore.ParameterSet.Config as cms

GenMatchProducer = cms.EDProducer("GenMatchProducer",
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("SimonGenJets"),
    dRthresh = cms.double(0.2),
    clipval = cms.double(0.05),
    cutoff = cms.double(2.0),
    matchCharge = cms.bool(True),
    maxReFit = cms.uint32(50),
    verbose = cms.int32(0)
)