import FWCore.ParameterSet.Config as cms
import numpy as np
from SRothman.Analysis.config.config import config

res4calculator_reco = cms.PSet(
    tolerance = cms.double(config['EECs']['tolerance']),
    tri_tolerance = cms.double(config['EECs']['tri_tolerance']),
    normType = cms.string("RAWPT"),
    bins = cms.PSet(
        R =          cms.vdouble(config['EECs']['res4bins']['reco']['R']),
        r_dipole =   cms.vdouble(config['EECs']['res4bins']['reco']['r_dipole']),
        c_dipole =   cms.vdouble(config['EECs']['res4bins']['reco']['c_dipole']),
        r_tee =      cms.vdouble(config['EECs']['res4bins']['reco']['r_tee']),
        c_tee =      cms.vdouble(config['EECs']['res4bins']['reco']['c_tee']),
        r_triangle = cms.vdouble(config['EECs']['res4bins']['reco']['r_triangle']),
        c_triangle = cms.vdouble(config['EECs']['res4bins']['reco']['c_triangle'])
    )
)

res4calculator_gen = cms.PSet(
    tolerance = cms.double(config['EECs']['tolerance']),
    tri_tolerance = cms.double(config['EECs']['tri_tolerance']),
    normType = cms.string("RAWPT"),
    bins = cms.PSet(
        R =          cms.vdouble(config['EECs']['res4bins']['gen']['R']),
        r_dipole =   cms.vdouble(config['EECs']['res4bins']['gen']['r_dipole']),
        c_dipole =   cms.vdouble(config['EECs']['res4bins']['gen']['c_dipole']),
        r_tee =      cms.vdouble(config['EECs']['res4bins']['gen']['r_tee']),
        c_tee =      cms.vdouble(config['EECs']['res4bins']['gen']['c_tee']),
        r_triangle = cms.vdouble(config['EECs']['res4bins']['gen']['r_triangle']),
        c_triangle = cms.vdouble(config['EECs']['res4bins']['gen']['c_triangle'])
    )
)

res4transfercalculator = cms.PSet(
    tolerance = cms.double(config['EECs']['tolerance']),
    tri_tolerance = cms.double(config['EECs']['tri_tolerance']),
    normType = cms.string("RAWPT"),
    bins_reco = cms.PSet(
        R =          cms.vdouble(config['EECs']['res4bins']['reco']['R']),
        r_dipole =   cms.vdouble(config['EECs']['res4bins']['reco']['r_dipole']),
        c_dipole =   cms.vdouble(config['EECs']['res4bins']['reco']['c_dipole']),
        r_tee =      cms.vdouble(config['EECs']['res4bins']['reco']['r_tee']),
        c_tee =      cms.vdouble(config['EECs']['res4bins']['reco']['c_tee']),
        r_triangle = cms.vdouble(config['EECs']['res4bins']['reco']['r_triangle']),
        c_triangle = cms.vdouble(config['EECs']['res4bins']['reco']['c_triangle'])
    ),
    bins_gen = cms.PSet(
        R =          cms.vdouble(config['EECs']['res4bins']['gen']['R']),
        r_dipole =   cms.vdouble(config['EECs']['res4bins']['gen']['r_dipole']),
        c_dipole =   cms.vdouble(config['EECs']['res4bins']['gen']['c_dipole']),
        r_tee =      cms.vdouble(config['EECs']['res4bins']['gen']['r_tee']),
        c_tee =      cms.vdouble(config['EECs']['res4bins']['gen']['c_tee']),
        r_triangle = cms.vdouble(config['EECs']['res4bins']['gen']['r_triangle']),
        c_triangle = cms.vdouble(config['EECs']['res4bins']['gen']['c_triangle'])
    )
)
