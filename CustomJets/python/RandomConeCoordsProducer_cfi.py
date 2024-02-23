import FWCore.ParameterSet.Config as cms

RandomConeCoordsProducer = cms.EDProducer("RandomConeCoordsProducer",
    maxEta = cms.double(1.7),
    verbose = cms.int32(0)
)
