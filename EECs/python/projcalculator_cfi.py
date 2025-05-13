import FWCore.ParameterSet.Config as cms
import numpy as np
from SRothman.Analysis.config.config import config

projcalculator = cms.PSet(
    normType = cms.string("RAWPT"),
    bins = cms.PSet(
        R = cms.vdouble([0,1]),
    )
)

projtransfercalculator = cms.PSet(
    normType = cms.string("RAWPT"),
    bins_reco = cms.PSet(
        R = cms.vdouble([0,1]),
    ),
    bins_gen = cms.PSet(
        R = cms.vdouble([0,1]),
    )
)
