import FWCore.ParameterSet.Config as cms
import numpy as np

R_bins_reco = np.linspace(0, 1.0, 11)

r_dipole_bins_reco = np.linspace(0, 1.0, 21)
c_dipole_bins_reco = np.linspace(0, np.pi/2, 21)

r_tee_bins_reco = np.linspace(0, 1.0, 21)
c_tee_bins_reco = np.linspace(0, np.pi/2, 21)

r_triangle_bins_reco = np.linspace(0, 1.0, 21)
c_triangle_bins_reco = np.linspace(0, 2*np.pi, 41)

R_bins_gen = np.linspace(0, 1.0, 11)

r_dipole_bins_gen = np.linspace(0, 1.0, 21)
c_dipole_bins_gen = np.linspace(0, np.pi/2, 21)

r_tee_bins_gen = np.linspace(0, 1.0, 21)
c_tee_bins_gen = np.linspace(0, np.pi/2, 21)

r_triangle_bins_gen = np.linspace(0, 1.0, 21)
c_triangle_bins_gen = np.linspace(0, 2*np.pi, 41)

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

