import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.ZMuMuEventSelectionProducer_cfi import *

def addZMuMuEventSelection(process, verbose=False):
    process.ZMuMu = ZMuMuEventSelectionProducer.clone(
        src = 'finalMuons',
        verbose = verbose
    )
    process.ZMuMUTask = cms.Task(process.ZMuMu)
    process.schedule.associate(process.ZMuMUTask)
    return process
