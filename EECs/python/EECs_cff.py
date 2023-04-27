import FWCore.ParameterSet.Config as cms
from SRothman.EECs.EECProducer_cfi import *
from SRothman.EECs.EECTableProducer_cfi import *

def addEECs(process):
    process.EEC = EECProducer.clone(
        reco = "SimonJets",
        gen = "GenSimonJets",
        match = "GenMatch",
    )
    process.RecoEECTable = EECTableProducer.clone(
        src = "EEC:reco",
        name = "RecoEEC"
    )
    process.GenEECTable = EECTableProducer.clone(
        src = "EEC:gen",
        name =  "GenEEC"
    )
    process.EECTask = cms.Task(process.EEC)
    process.EECTableTask = cms.Task(process.RecoEECTable,
                                    process.GenEECTable)
    process.schedule.associate(process.EECTask)
    process.schedule.associate(process.EECTableTask)

    return process
