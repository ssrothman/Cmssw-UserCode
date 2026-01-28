import FWCore.ParameterSet.Config as cms
import numpy as np

def get_proj_calculator(config, genreco):
    edges = config['edges_gen'] if genreco == 'gen' else config['edges_reco']
    return cms.PSet(
        normType = cms.string(config['normType']),
        bins = cms.PSet(
            R = cms.vdouble(edges['R']),
        )
    )

def get_proj_transfer_calculator(config):
    return cms.PSet(
        normType = cms.string(config['normType']),
        bins_reco = cms.PSet(
            R = cms.vdouble(config['edges_reco']['R']),
        ),
        bins_gen = cms.PSet(
            R = cms.vdouble(config['edges_gen']['R']),
        )
    )