import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *

from SRothman.CustomJets.SVmatchTableProducer_cfi import *

def setupCHSJets(process):

    process.SVmatchTable = SVMatchTableProducer.clone()

    process.SVmatchTableTask = cms.Task(
        process.SVmatchTable,
    )

    process.schedule.associate(process.SVmatchTableTask)

    return process

