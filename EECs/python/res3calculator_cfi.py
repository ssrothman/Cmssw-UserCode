import FWCore.ParameterSet.Config as cms
import numpy as np
from SRothman.Analysis.config.config import config

def get_res3_calculator(config, genreco):
    edges = config['edges_gen'] if genreco == 'gen' else config['edges_reco']
    return cms.PSet(
        normType = cms.string(config['normType']),
        bins = cms.PSet(
            R = cms.vdouble(edges['R']),
            r = cms.vdouble(edges['r']),
            c = cms.vdouble(edges['c']),
        )
    )

def get_res3_transfer_calculator(config):
    return cms.PSet(
        normType = cms.string(config['normType']),
        bins_reco = cms.PSet(
            R = cms.vdouble(config['edges_reco']['R']),
            r = cms.vdouble(config['edges_reco']['r']),
            c = cms.vdouble(config['edges_reco']['c']),
        ),
        bins_gen = cms.PSet(
            R = cms.vdouble(config['edges_gen']['R']),
            r = cms.vdouble(config['edges_gen']['r']),
            c = cms.vdouble(config['edges_gen']['c']),
        )
    )