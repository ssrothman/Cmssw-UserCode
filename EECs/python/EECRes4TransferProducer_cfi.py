import FWCore.ParameterSet.Config as cms

import bins

EECRes4TransferProducer = cms.EDProducer("EECRes4TransferProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = cms.PSet(
        tolerance = cms.double(0.05),
        tri_tolerance = cms.double(0.05),
        normType = cms.string("RAWPT"),
        bins_reco = cms.PSet(
            R = cms.vdouble(bins.R_bins_reco),
            r_dipole = cms.vdouble(bins.r_dipole_bins_reco),
            c_dipole = cms.vdouble(bins.c_dipole_bins_reco),
            r_tee = cms.vdouble(bins.r_tee_bins_reco),
            c_tee = cms.vdouble(bins.c_tee_bins_reco),
            r_triangle = cms.vdouble(bins.r_triangle_bins_reco),
            c_triangle = cms.vdouble(bins.c_triangle_bins_reco)
        ),
        bins_gen = cms.PSet(
            R = cms.vdouble(bins.R_bins_gen),
            r_dipole = cms.vdouble(bins.r_dipole_bins_gen),
            c_dipole = cms.vdouble(bins.c_dipole_bins_gen),
            r_tee = cms.vdouble(bins.r_tee_bins_gen),
            c_tee = cms.vdouble(bins.c_tee_bins_gen),
            r_triangle = cms.vdouble(bins.r_triangle_bins_gen),
            c_triangle = cms.vdouble(bins.c_triangle_bins_gen)
        )
    )
)
