import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.ZMuMuEventSelectionProducer_cfi import *

def setupZMuMu(process):
    process.ZMuMu = ZMuMuEventSelectionProducer.clone(
        src = 'finalMuons',
        verbose = False
    )
    process.ZMuMUTask = cms.Task(process.ZMuMu)
    process.schedule.associate(process.ZMuMUTask)
    return process
