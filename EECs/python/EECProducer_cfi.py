import FWCore.ParameterSet.Config as cms

import numpy as np

dRedges = np.linspace(0, 0.5, 51)
dRedges[0] = 1e-10

EECProducer = cms.EDProducer("EECProducer",
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("GenSimonJets"),
    match = cms.InputTag("GenMatch"),
    doGen = cms.bool(True),

    maxOrder = cms.uint32(6),
    doRes3 = cms.bool(True),
    doRes4 = cms.bool(True),
    p1s = cms.vuint32(2, 2, 3),
    p2s = cms.vuint32(1, 2, 1),

    verbose = cms.int32(1),

    dRbinEdges = cms.vdouble(*dRedges), 
)
