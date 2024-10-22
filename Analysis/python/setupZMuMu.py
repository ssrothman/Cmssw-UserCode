import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.ZMuMuEventSelectionProducer_cfi import *
from SRothman.Analysis.GenZDecayEventSelectionProducer_cfi import *

def setupZMuMu(process):
    process.ZMuMu = ZMuMuEventSelectionProducer.clone(
        src = 'finalMuons',
        verbose = False
    )
    process.ZMuMUTask = cms.Task(process.ZMuMu)
    process.schedule.associate(process.ZMuMUTask)
    return process

def setupGenZDecay(process):
    process.GenZDecay = GenZDecayEventSelectionProducer.clone()
    process.GenZDecayTask = cms.Task(process.GenZDecay)
    process.schedule.associate(process.GenZDecayTask)
    return process
