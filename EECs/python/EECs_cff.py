import FWCore.ParameterSet.Config as cms
from SRothman.EECs.EECProducer_cfi import *
from SRothman.EECs.EECTableProducer_cfi import *
from SRothman.EECs.EECTransferTableProducer_cfi import *

def addEECs(process, verbose,
            name, genMatch):

    setattr(process, name,
        EECProducer.clone(
            reco = "SimonJets",
            gen = "GenSimonJets",
            match = genMatch,
            maxOrder = 3,
            doRes4 = False,
            doRes3 = False,
            p1s = [1],
            p2s = [2],
            verbose = verbose
        )
    )
    setattr(process, "Reco%sTable"%name,
        EECTableProducer.clone(
            src = "%s:reco"%name,
            name = "Reco%s"%name,
            verbose = 0,
        )
    )
    setattr(process, "Reco%sPUTable"%name,
        EECTableProducer.clone(
            src = "%s:recoPU"%name,
            name = "Reco%sPU"%name,
            verbose = 0,
        )
    )
    setattr(process, 'Gen%sTable'%name,
        EECTableProducer.clone(
            src = "%s:gen"%name,
            name =  "Gen%s"%name,
            verbose = 0,
        )
    )
    setattr(process, 'Gen%sUNMATCHTable'%name,
        EECTableProducer.clone(
            src = "%s:genUNMATCH"%name,
            name =  "Gen%sUNMATCH"%name,
            verbose = 0,
        )
    )
    setattr(process,'Transfer%sTable'%name,
        EECTransferTableProducer.clone(
            src = "%s:transfer"%name,
            name = "%sTransfer"%name,
            verbose = 0,
        )
    )
    setattr(process, '%sTask'%name, 
            cms.Task(getattr(process,'%s'%name)))
    setattr(process, '%sTableTask'%name,
        cms.Task(
            getattr(process, 'Reco%sTable'%name),
            getattr(process, 'Reco%sPUTable'%name),
            getattr(process, 'Gen%sTable'%name),
            getattr(process, 'Gen%sUNMATCHTable'%name),
            getattr(process, 'Transfer%sTable'%name),
        )
    )
    process.schedule.associate(getattr(process, '%sTask'%name))
    process.schedule.associate(getattr(process, '%sTableTask'%name))

    return process
