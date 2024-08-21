import FWCore.ParameterSet.Config as cms

import numpy as np

#dRedges = np.linspace(0, 0.5, 51)
#dRedges[0] = 1e-10
dRedges = [0.0005 ,0.001, 0.005, 
           0.01, 0.015, 0.02, 0.025,
           0.03, 0.035, 0.04, 0.045, 
           0.05, 0.06,  
           0.07, 0.08,
           0.09, 0.10, 
           0.11, 0.13, 0.15,
           0.20, 0.25, 0.30, 0.35,
           0.40, 0.50, 0.60, 
           0.70, 0.80, 1.00]

dRedges_coarse = [0.0, 0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80]

xiedges = np.linspace(1e-5, 1.0+1e-5, 15)
phiedges = np.linspace(1e-5, np.pi/2 + 1e-5, 15)

r_dipole_edges = np.linspace(0, 1, 15)
ct_dipole_edges = np.linspace(0, np.pi/2, 15)

r_tee_edges = np.linspace(0, 1, 15)
ct_tee_edges = np.linspace(0, np.pi/2, 15)

r_triangle_edges = np.linspace(0, 1, 15)
ct_triangle_edges = np.linspace(0, np.pi/2, 15)

EECProducer = cms.EDProducer("EECProducer",
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("GenSimonJets"),
    match = cms.InputTag("GenMatch"),

    maxOrder = cms.uint32(6),
    doRes3 = cms.bool(True),
    doRes4 = cms.bool(True),
    doRes4Fixed = cms.bool(False),
    doTransfer= cms.bool(True),

    ptNorm = cms.string("RAW"),

    doGen = cms.bool(True),

    verbose = cms.int32(0),

    dRbinEdges = cms.vdouble(*dRedges), 

    dRbinEdges_coarse = cms.vdouble(*dRedges_coarse),

    xibinEdges = cms.vdouble(*xiedges),
    phibinEdges = cms.vdouble(*phiedges),

    r_dipole_edges = cms.vdouble(*r_dipole_edges),
    ct_dipole_edges = cms.vdouble(*ct_dipole_edges),

    r_tee_edges = cms.vdouble(*r_tee_edges),
    ct_tee_edges = cms.vdouble(*ct_tee_edges),

    r_triangle_edges = cms.vdouble(*r_triangle_edges),
    ct_triangle_edges = cms.vdouble(*ct_triangle_edges),

    shapetol = cms.double(0.05),
)
