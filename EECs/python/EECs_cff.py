import FWCore.ParameterSet.Config as cms
from SRothman.EECs.EECProducer_cfi import *
from SRothman.EECs.EECTableProducer_cfi import *
from SRothman.EECs.EECTransferTableProducer_cfi import *

def addEECs(process, verbose=False):
    process.EEC = EECProducer.clone(
        reco = "SimonJets",
        gen = "GenSimonJets",
        match = "GenMatch",
        maxOrder = 2,
        doRes4 = False,
        doRes3 = True,
        p1s = [1],
        p2s = [2],
        verbose = verbose
    )
    process.RecoEECTable = EECTableProducer.clone(
        src = "EEC:reco",
        name = "RecoEEC",
        verbose = verbose
    )
    process.GenEECTable = EECTableProducer.clone(
        src = "EEC:gen",
        name =  "GenEEC",
        verbose = verbose
    )
    process.TransferEECTable = EECTransferTableProducer.clone(
        src = "EEC:transfer",
        name = "EECTransfer",
        verbose = verbose
    )
    process.EECTask = cms.Task(process.EEC)
    process.EECTableTask = cms.Task(process.RecoEECTable,
                                    process.GenEECTable,
                                    process.TransferEECTable)
    process.schedule.associate(process.EECTask)
    process.schedule.associate(process.EECTableTask)

    return process
