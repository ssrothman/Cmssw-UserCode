import FWCore.ParameterSet.Config as cms

import bins

EECRes4Producer = cms.EDProducer("EECRes4Producer",
    jets = cms.InputTag(""),
    calculator = cms.PSet(
        tolerance = cms.double(0.05),
        tri_tolerance = cms.double(0.05),
        normType = cms.string("RAWPT"),
        bins = cms.PSet(
            R = cms.vdouble(bins.R_bins_reco),
            r_dipole = cms.vdouble(bins.r_dipole_bins_reco),
            c_dipole = cms.vdouble(bins.c_dipole_bins_reco),
            r_tee = cms.vdouble(bins.r_tee_bins_reco),
            c_tee = cms.vdouble(bins.c_tee_bins_reco),
            r_triangle = cms.vdouble(bins.r_triangle_bins_reco),
            c_triangle = cms.vdouble(bins.c_triangle_bins_reco)
        )
    )
)
