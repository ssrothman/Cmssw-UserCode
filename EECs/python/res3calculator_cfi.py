import FWCore.ParameterSet.Config as cms
import numpy as np
from SRothman.Analysis.config.config import config

res3calculator = cms.PSet(
    normType = cms.string("RAWPT"),
    bins = cms.PSet(
        R = cms.vdouble([0,1]),
        r = cms.vdouble([0,1]),
        c = cms.vdouble([0,1]),
    )
)

res3transfercalculator = cms.PSet(
    normType = cms.string("RAWPT"),
    bins_reco = cms.PSet(
        R = cms.vdouble([0,1]),
        r = cms.vdouble([0,1]),
        c = cms.vdouble([0,1])
    ),
    bins_gen = cms.PSet(
        R = cms.vdouble([0,1]),
        r = cms.vdouble([0,1]),
        c = cms.vdouble([0,1]),
    )
)
