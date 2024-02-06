import FWCore.ParameterSet.Config as cms

import numpy as np

#dRedges = np.linspace(0, 0.5, 51)
#dRedges[0] = 1e-10
dRedges = [0.0001 ,0.001, 0.005, 
           0.01, 0.015, 0.02, 0.025,
           0.03, 0.035, 0.04, 0.045, 
           0.05, 0.06, 0.07, 0.08,
           0.09, 0.10, 0.15, 0.20, 
           0.30, 0.40, 0.50]

EECProducer = cms.EDProducer("EECProducer",
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("GenSimonJets"),
    match = cms.InputTag("GenMatch"),
    doGen = cms.bool(True),

    maxOrder = cms.uint32(6),
    doRes3 = cms.bool(False),
    doRes4 = cms.bool(False),
    p1s = cms.vuint32(),
    p2s = cms.vuint32(),

    normToRaw = cms.bool(True),

    verbose = cms.int32(1),

    dRbinEdges = cms.vdouble(*dRedges), 
)
