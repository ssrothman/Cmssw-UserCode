import FWCore.ParameterSet.Config as cms

from PhysicsTools.NanoAOD.globals_cff import genTable
from PhysicsTools.NanoAOD.lheInfoTable_cfi import *
from PhysicsTools.NanoAOD.genWeightsTable_cfi import *

def setupGenTables(process):
    process.genTable = genTable.clone(

    )
    process.lheInfoTable = lheInfoTable.clone(
        storeLHEParticles = cms.bool(True),
    )
    process.genWeightsTable = genWeightsTable.clone(

    )
    
    process.genTablesTask = cms.Task(process.genTable, process.lheInfoTable, process.genWeightsTable)
    process.schedule.associate(process.genTablesTask)

    return process
