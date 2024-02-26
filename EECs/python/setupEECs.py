import FWCore.ParameterSet.Config as cms
from SRothman.EECs.EECProducer_cfi import *
from SRothman.EECs.EECTableProducer_cfi import *
from SRothman.EECs.EECTransferTableProducer_cfi import *

def setupEECs(process, 
              name, genMatch,
              genjets, recojets,
              verbose=0,
              isMC = True):

    setattr(process, name,
        EECProducer.clone(
            reco = recojets,
            gen = genjets,
            match = genMatch,
            verbose = verbose,
            doGen = isMC,
        )
    )
    setattr(process, "Reco%sTable"%name,
        EECTableProducer.clone(
            src = "%s:reco"%name,
            name = "Reco%s"%name,
            verbose = verbose,
        )
    )
    if isMC:
        setattr(process, "Reco%sPUTable"%name,
            EECTableProducer.clone(
                src = "%s:recoPU"%name,
                name = "Reco%sPU"%name,
                verbose = verbose,
            )
        )
        setattr(process, 'Gen%sTable'%name,
            EECTableProducer.clone(
                src = "%s:gen"%name,
                name =  "Gen%s"%name,
                verbose = verbose,
            )
        )
        setattr(process, 'Gen%sUNMATCHTable'%name,
            EECTableProducer.clone(
                src = "%s:genUNMATCH"%name,
                name =  "Gen%sUNMATCH"%name,
                verbose = verbose,
            )
        )
        setattr(process,'Transfer%sTable'%name,
            EECTransferTableProducer.clone(
                src = "%s:transfer"%name,
                name = "%sTransfer"%name,
                verbose = verbose,
            )
        )
    setattr(process, '%sTask'%name, 
            cms.Task(getattr(process,'%s'%name)))
    setattr(process, '%sTableTask'%name,
        cms.Task(
            getattr(process, 'Reco%sTable'%name),
        )
    )
    process.schedule.associate(getattr(process, '%sTask'%name))
    process.schedule.associate(getattr(process, '%sTableTask'%name))

    if isMC:
        setattr(process, "%sMCTableTask"%name,
            cms.Task(
                getattr(process, 'Reco%sPUTable'%name),
                getattr(process, 'Gen%sTable'%name),
                getattr(process, 'Gen%sUNMATCHTable'%name),
                getattr(process, 'Transfer%sTable'%name),
            )
        )
        #process.schedule.associate(getattr(process, '%sMCTableTask'%name))

    return process
