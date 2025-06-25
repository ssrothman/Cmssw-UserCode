import FWCore.ParameterSet.Config as cms
import numpy as np
from SRothman.Analysis.config.config import config

res3calculator_reco = cms.PSet(
    normType = cms.string("RAWPT"),
    bins = cms.PSet(
        R = cms.vdouble(config['EECs']['res3bins']['reco']['R']),
        r = cms.vdouble(config['EECs']['res3bins']['reco']['r']),
        c = cms.vdouble(config['EECs']['res3bins']['reco']['c']),
    )
)

res3calculator_gen = cms.PSet(
    normType = cms.string("RAWPT"),
    bins = cms.PSet(
        R = cms.vdouble(config['EECs']['res3bins']['gen']['R']),
        r = cms.vdouble(config['EECs']['res3bins']['gen']['r']),
        c = cms.vdouble(config['EECs']['res3bins']['gen']['c']),
    )
)

res3transfercalculator = cms.PSet(
    normType = cms.string("RAWPT"),
    bins_reco = cms.PSet(
        R = cms.vdouble(config['EECs']['res3bins']['reco']['R']),
        r = cms.vdouble(config['EECs']['res3bins']['reco']['r']),
        c = cms.vdouble(config['EECs']['res3bins']['reco']['c']),
    ),
    bins_gen = cms.PSet(
        R = cms.vdouble(config['EECs']['res3bins']['gen']['R']),
        r = cms.vdouble(config['EECs']['res3bins']['gen']['r']),
        c = cms.vdouble(config['EECs']['res3bins']['gen']['c']),
    )
)
