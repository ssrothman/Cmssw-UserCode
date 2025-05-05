import FWCore.ParameterSet.Config as cms
import numpy as np
from SRothman.Analysis.config.config import config

res4calculator = cms.PSet(
    tolerance = cms.double(config['EECs']['tolerance']),
    tri_tolerance = cms.double(config['EECs']['tri_tolerance']),
    normType = cms.string("RAWPT"),
    bins = cms.PSet(
        R = cms.vdouble([0,1]),
        r_dipole = cms.vdouble([0,1]),
        c_dipole = cms.vdouble([0,1]),
        r_tee = cms.vdouble([0,1]),
        c_tee = cms.vdouble([0,1]),
        r_triangle = cms.vdouble([0,1]),
        c_triangle = cms.vdouble([0,1])
    )
)

res4transfercalculator = cms.PSet(
    tolerance = cms.double(config['EECs']['tolerance']),
    tri_tolerance = cms.double(config['EECs']['tri_tolerance']),
    normType = cms.string("RAWPT"),
    bins_reco = cms.PSet(
        R = cms.vdouble([0,1]),
        r_dipole = cms.vdouble([0,1]),
        c_dipole = cms.vdouble([0,1]),
        r_tee = cms.vdouble([0,1]),
        c_tee = cms.vdouble([0,1]),
        r_triangle = cms.vdouble([0,1]),
        c_triangle = cms.vdouble([0,1])
    ),
    bins_gen = cms.PSet(
        R = cms.vdouble([0,1]),
        r_dipole = cms.vdouble([0,1]),
        c_dipole = cms.vdouble([0,1]),
        r_tee = cms.vdouble([0,1]),
        c_tee = cms.vdouble([0,1]),
        r_triangle = cms.vdouble([0,1]),
        c_triangle = cms.vdouble([0,1])
    )
)
