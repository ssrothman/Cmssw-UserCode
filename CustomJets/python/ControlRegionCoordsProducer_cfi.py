import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.config.config import config

ControlRegionCoordsProducer = cms.EDProducer("ControlRegionCoordsProducer",
    maxEta = cms.double(config['Jets']['JetEta']),
    verbose = cms.int32(0),
    muonSrc = cms.InputTag("slimmedMuons"),
)
