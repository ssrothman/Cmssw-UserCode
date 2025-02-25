import FWCore.ParameterSet.Config as cms
import numpy as np

R_bins_reco = np.linspace(0, 1.0, 11)

r_dipole_bins_reco = np.linspace(0, 1.0, 21)
c_dipole_bins_reco = np.linspace(0, np.pi/2, 21)

r_tee_bins_reco = np.linspace(0, 1.0, 21)
c_tee_bins_reco = np.linspace(0, np.pi/2, 21)

r_triangle_bins_reco = np.linspace(0, 2.0, 31)
c_triangle_bins_reco = np.linspace(-np.pi, np.pi, 41)

R_bins_gen = np.linspace(0, 1.0, 11)

r_dipole_bins_gen = np.linspace(0, 1.0, 21)
c_dipole_bins_gen = np.linspace(0, np.pi/2, 21)

r_tee_bins_gen = np.linspace(0, 1.0, 21)
c_tee_bins_gen = np.linspace(0, np.pi/2, 21)

r_triangle_bins_gen = np.linspace(0, 2.0, 31)
c_triangle_bins_gen = np.linspace(-np.pi, np.pi, 41)

res4calculator = cms.PSet(
    tolerance = cms.double(0.05),
    tri_tolerance = cms.double(0.05),
    normType = cms.string("RAWPT"),
    bins = cms.PSet(
        R = cms.vdouble(R_bins_reco),
        r_dipole = cms.vdouble(r_dipole_bins_reco),
        c_dipole = cms.vdouble(c_dipole_bins_reco),
        r_tee = cms.vdouble(r_tee_bins_reco),
        c_tee = cms.vdouble(c_tee_bins_reco),
        r_triangle = cms.vdouble(r_triangle_bins_reco),
        c_triangle = cms.vdouble(c_triangle_bins_reco)
    )
)

res4transfercalculator = cms.PSet(
    tolerance = cms.double(0.05),
    tri_tolerance = cms.double(0.05),
    normType = cms.string("RAWPT"),
    bins_reco = cms.PSet(
        R = cms.vdouble(R_bins_reco),
        r_dipole = cms.vdouble(r_dipole_bins_reco),
        c_dipole = cms.vdouble(c_dipole_bins_reco),
        r_tee = cms.vdouble(r_tee_bins_reco),
        c_tee = cms.vdouble(c_tee_bins_reco),
        r_triangle = cms.vdouble(r_triangle_bins_reco),
        c_triangle = cms.vdouble(c_triangle_bins_reco)
    ),
    bins_gen = cms.PSet(
        R = cms.vdouble(R_bins_gen),
        r_dipole = cms.vdouble(r_dipole_bins_gen),
        c_dipole = cms.vdouble(c_dipole_bins_gen),
        r_tee = cms.vdouble(r_tee_bins_gen),
        c_tee = cms.vdouble(c_tee_bins_gen),
        r_triangle = cms.vdouble(r_triangle_bins_gen),
        c_triangle = cms.vdouble(c_triangle_bins_gen)
    )
)
