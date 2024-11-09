import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.config.config import config

RandomConeCoordsProducer = cms.EDProducer("RandomConeCoordsProducer",
    maxEta = cms.double(config['Jets']['JetEta']),
    verbose = cms.int32(0)
)
