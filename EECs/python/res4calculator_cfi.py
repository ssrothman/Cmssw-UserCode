import FWCore.ParameterSet.Config as cms
import numpy as np
from SRothman.Analysis.config.config import config

def get_res4_calculator(config, genreco):
    edges = config['edges_gen'] if genreco == 'gen' else config['edges_reco']
    return cms.PSet(
        tolerance = cms.double(config['tolerance']),
        tri_tolerance = cms.double(config['tri_tolerance']),
        normType = cms.string(config['normType']),
        bins = cms.PSet(
            R =          cms.vdouble(edges['R']),
            r_dipole =   cms.vdouble(edges['r_dipole']),
            c_dipole =   cms.vdouble(edges['c_dipole']),
            r_tee =      cms.vdouble(edges['r_tee']),
            c_tee =      cms.vdouble(edges['c_tee']),
            r_triangle = cms.vdouble(edges['r_triangle']),
            c_triangle = cms.vdouble(edges['c_triangle'])
        )
    )

def get_res4_transfer_calculator(config):
    return cms.PSet(
        tolerance = cms.double(config['tolerance']),
        tri_tolerance = cms.double(config['tri_tolerance']),
        normType = cms.string(config['normType']),
        bins_reco = cms.PSet(
            R =          cms.vdouble(config['edges_reco']['R']),
            r_dipole =   cms.vdouble(config['edges_reco']['r_dipole']),
            c_dipole =   cms.vdouble(config['edges_reco']['c_dipole']),
            r_tee =      cms.vdouble(config['edges_reco']['r_tee']),
            c_tee =      cms.vdouble(config['edges_reco']['c_tee']),
            r_triangle = cms.vdouble(config['edges_reco']['r_triangle']),
            c_triangle = cms.vdouble(config['edges_reco']['c_triangle'])
        ),
        bins_gen = cms.PSet(
            R =          cms.vdouble(config['edges_gen']['R']),
            r_dipole =   cms.vdouble(config['edges_gen']['r_dipole']),
            c_dipole =   cms.vdouble(config['edges_gen']['c_dipole']),
            r_tee =      cms.vdouble(config['edges_gen']['r_tee']),
            c_tee =      cms.vdouble(config['edges_gen']['c_tee']),
            r_triangle = cms.vdouble(config['edges_gen']['r_triangle']),
            c_triangle = cms.vdouble(config['edges_gen']['c_triangle'])
        )
    )