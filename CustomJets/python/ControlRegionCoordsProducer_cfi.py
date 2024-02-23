import FWCore.ParameterSet.Config as cms

ControlRegionCoordsProducer = cms.EDProducer("ControlRegionCoordsProducer",
    maxEta = cms.double(1.7),
    verbose = cms.int32(0),
    muonSrc = cms.InputTag("slimmedMuons"),
)

