import FWCore.ParameterSet.Config as cms
import numpy as np
from SRothman.Analysis.config.config import config

projcalculator_reco = cms.PSet(
    normType = cms.string("RAWPT"),
    bins = cms.PSet(
        R = cms.vdouble(config['EECs']['projbins']['reco']['R']),
    )
)
projcalculator_gen = cms.PSet(
    normType = cms.string("RAWPT"),
    bins = cms.PSet(
        R = cms.vdouble(config['EECs']['projbins']['gen']['R']),
    )
)

projtransfercalculator = cms.PSet(
    normType = cms.string("RAWPT"),
    bins_reco = cms.PSet(
        R = cms.vdouble(config['EECs']['projbins']['reco']['R']),
    ),
    bins_gen = cms.PSet(
        R = cms.vdouble(config['EECs']['projbins']['gen']['R']),
    )
)
