import FWCore.ParameterSet.Config as cms
from SRothman.EECs.EECProducer_cfi import *
from SRothman.EECs.EECTableProducer_cfi import *
from SRothman.EECs.EECTransferTableProducer_cfi import *

def addEECs(process):
    process.EEC = EECProducer.clone(
        reco = "SimonJets",
        gen = "GenSimonJets",
        match = "GenMatch",
        doRes4 = False,
    )
    process.RecoEECTable = EECTableProducer.clone(
        src = "EEC:reco",
        name = "RecoEEC"
    )
    process.GenEECTable = EECTableProducer.clone(
        src = "EEC:gen",
        name =  "GenEEC"
    )
    process.TransferEECTable = EECTransferTableProducer.clone(
        src = "EEC:transfer",
        name = "EECTransfer"
    )
    process.EECTask = cms.Task(process.EEC)
    process.EECTableTask = cms.Task(process.RecoEECTable,
                                    process.GenEECTable,
                                    process.TransferEECTable)
    process.schedule.associate(process.EECTask)
    process.schedule.associate(process.EECTableTask)

    return process
