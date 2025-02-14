import FWCore.ParameterSet.Config as cms

from SRothman.EECs.EECRes4Producer_cfi import *
from SRothman.EECs.EECRes4TransferProducer_cfi import *
from SRothman.EECs.EECRes4TableProducer_cfi import *
from SRothman.EECs.EECRes4TransferTableProducer_cfi import *

def setupEECRes4(process, 
              name, genMatch,
              genjets, recojets,
              verbose=0,
              isMC = True):

    setattr(process, name,
        EECRes4Producer.clone(
            jets = recojets
        )
    )
    setattr(process, "Reco%sTable"%name,
        EECRes4TableProducer.clone(
            EECs = name,
            name = "Reco%s"%name,
        )
    )

    setattr(process, '%sTask'%name, cms.Task(
        getattr(process,'%s'%name),
        getattr(process, 'Reco%sTable'%name),
    ))

    process.schedule.associate(getattr(process, '%sTask'%name))

    if isMC:
        setattr(process, 'Gen%s'%name,
            EECRes4TransferProducer.clone(
                genJets = genjets,
                recoJets = recojets,
                matches = genMatch
            )
        )
        setattr(process, 'Gen%sTable'%name,
            EECRes4TableProducer.clone(
                EECs = "Gen%s:gen"%name,
                name =  "Gen%s"%name,
            )
        )
        setattr(process,'Transfer%sTable'%name,
            EECRes4TransferTableProducer.clone(
                EECTransfer = "Gen%s:transfer"%name,
                name = "%sTransfer"%name,
            )
        )
        setattr(process, 'UntransferedGen%sTable'%name,
            EECRes4TableProducer.clone(
                EECs = "Gen%s:untransferedGen"%name,
                name =  "UntransferedGen%s"%name,
            )
        )
        setattr(process, 'UntransferedReco%sTable'%name,
            EECRes4TableProducer.clone(
                EECs = "Gen%s:untransferedReco"%name,
                name =  "UntransferedReco%s"%name,
            )
        )

        setattr(process, "%sMCTask"%name,
            cms.Task(
                getattr(process, 'Gen%s'%name),
                getattr(process, 'Gen%sTable'%name),
                getattr(process, 'Transfer%sTable'%name),
                getattr(process, 'UntransferedGen%sTable'%name),
                getattr(process, 'UntransferedReco%sTable'%name),
            )
        )
        process.schedule.associate(getattr(process, '%sMCTask'%name))

    return process
