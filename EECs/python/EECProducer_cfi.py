import FWCore.ParameterSet.Config as cms

import numpy as np

#dRedges = np.linspace(0, 0.5, 51)
#dRedges[0] = 1e-10
dRedges = [0.0005 ,0.001, 0.005, 
           0.01, 0.015, 0.02, 0.025,
           0.03, 0.035, 0.04, 0.045, 
           0.05, 0.06, 0.07, 0.08,
           0.09, 0.10, 0.15, 0.20, 
           0.30, 0.40, 0.50, 0.60, 
           0.70, 0.80, 0.90, 1.00]

xi3edges = np.linspace(0.0, 1.0, 10)
phi3edges = np.linspace(0.0, 1.0, 10)

RM4edges = np.linspace(0.0, 1.0, 10)
phi4edges = np.linspace(-np.pi, np.pi, 10)

EECProducer = cms.EDProducer("EECProducer",
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("GenSimonJets"),
    match = cms.InputTag("GenMatch"),

    maxOrder = cms.uint32(6),
    doRes3 = cms.bool(True),
    doRes4 = cms.bool(True),
    ptNorm = cms.string("RAW"),

    doGen = cms.bool(True),

    verbose = cms.int32(1),

    dRbinEdges = cms.vdouble(*dRedges), 
    xi3binEdges = cms.vdouble(*xi3edges),
    phi3binEdges = cms.vdouble(*phi3edges),
    RM4binEdges = cms.vdouble(*RM4edges),
    phi4binEdges = cms.vdouble(*phi4edges),

    RMoRL = cms.double(1),
    RSoRL = cms.double(1),
    tol = cms.double(0.1),
)
