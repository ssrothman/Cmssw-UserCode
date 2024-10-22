import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.ZMuMuEventSelectionFilter_cfi import *

def setupZMuMu(process):
    process.ZMuMu = RECOZMuMuFilter.clone()
    return process

def setupGenZDecay(process):
    process.GenZDecay = GENZMuMuFilter.clone()
    return process
