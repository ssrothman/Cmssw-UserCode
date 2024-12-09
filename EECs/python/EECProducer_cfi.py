import FWCore.ParameterSet.Config as cms

import numpy as np

#dRedges = np.linspace(0, 0.5, 51)
#dRedges[0] = 1e-10

from SRothman.Analysis.config.config import config
dRedges = config['EECs']['DRedges']
dRedges_coarse = config['EECs']['DRedges_coarse']
xiedges = config['EECs']['xiedges']
phiedges = config['EECs']['phiedges']
r_dipole_edges = config['EECs']['r_dipole_edges']
ct_dipole_edges = config['EECs']['ct_dipole_edges']
r_tee_edges = config['EECs']['r_tee_edges']
ct_tee_edges = config['EECs']['ct_tee_edges']
r_triangle_edges = config['EECs']['r_triangle_edges']
ct_triangle_edges = config['EECs']['ct_triangle_edges']
r_minR_edges = config['EECs']['r_minR_edges']
phi_minR_edges = config['EECs']['phi_minR_edges']

EECProducer = cms.EDProducer("EECProducer",
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("GenSimonJets"),
    match = cms.InputTag("GenMatch"),

    maxOrder = cms.uint32(config['EECs']['MaxOrder']),
    doRes3 = cms.bool(config['EECs']['DoRes3']),
    doRes4 = cms.bool(config['EECs']['DoRes4']),
    doRes4Fixed = cms.bool(config['EECs']['DoRes4Fixed']),
    doTransfer= cms.bool(config['EECs']['DoTransfer']),

    ptNorm = cms.string(config['EECs']['pTNorm']),

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

    r_minR_edges = cms.vdouble(*r_minR_edges),
    phi_minR_edges = cms.vdouble(*phi_minR_edges),

    shapetol = cms.double(config['EECs']['ShapeTol']),
)
