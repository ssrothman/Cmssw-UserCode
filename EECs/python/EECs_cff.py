import FWCore.ParameterSet.Config as cms
from SRothman.EECs.EECProducer_cfi import *
from SRothman.EECs.EECTableProducer_cfi import *

def addEECs(process):
    process.EEC = RecoEECProducer.clone(
        reco = "SimonJets",
    )
    process.EECTable = EECTableProducer.clone(
        src = "EEC",
        name = "RecoEEC"
    )
    process.EECTask = cms.Task(process.EEC)
    process.EECTableTask = cms.Task(process.EECTable)
    process.schedule.associate(process.EECTask)
    process.schedule.associate(process.EECTableTask)

    return process
