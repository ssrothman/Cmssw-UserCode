import FWCore.ParameterSet.Config as cms

from SRothman.EECs.EECRes4Producer_cfi import *
from SRothman.EECs.EECRes4MatchedProducer_cfi import *
from SRothman.EECs.EECRes4TransferProducer_cfi import *
from SRothman.EECs.EECRes4TableProducer_cfi import *
from SRothman.EECs.EECRes4TransferTableProducer_cfi import *

def setupEECRes4_data(process,
                      name,
                      recojets,
                      flags=['Preselection', 'OverlapVeto'],
                      verbose=0):
    
    flags = [recojets + flag for flag in flags]

    setattr(process, name,
        EECRes4Producer.clone(
            jets = recojets,
            flags = flags,
        )
    )
    setattr(process, "Reco%sTable"%name,
        EECRes4TableProducer.clone(
            EECs = "%s:reco"%name,
            name = "Reco%s"%name,
        )
    )

    setattr(process, "Reco%sTask"%name,
        cms.Task(
            getattr(process, name),
            getattr(process, "Reco%sTable"%name),
        )
    )
    process.schedule.associate(getattr(process, "Reco%sTask"%name))

    return process

def setupEECRes4_MC(process, 
              name, genMatch,
              genjets, recojets,
              flags=['Preselection', 'OverlapVeto'],
              verbose=0):

    flags = [recojets + flag for flag in flags]

    setattr(process, name,
        EECRes4MatchedProducer.clone(
            jets = recojets,
            matches = genMatch,
            flags = flags
        )
    )
    setattr(process, "Reco%sTable"%name,
        EECRes4TableProducer.clone(
            EECs = "%s:reco"%name,
            name = "Reco%s"%name,
        )
    )
    setattr(process, "UnmatchedReco%sTable"%name,
        EECRes4TableProducer.clone(
            EECs = "%s:unmatchedReco"%name,
            name = "UnmatchedReco%s"%name,
        )
    )

    setattr(process, '%sRecoTask'%name, cms.Task(
        getattr(process,'%s'%name),
        getattr(process, 'Reco%sTable'%name),
        getattr(process, 'UnmatchedReco%sTable'%name),
    ))

    process.schedule.associate(getattr(process, '%sRecoTask'%name))

    setattr(process, 'Gen%s'%name,
        EECRes4TransferProducer.clone(
            genJets = genjets,
            recoJets = recojets,
            matches = genMatch,
            flags = flags,
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
    setattr(process, 'UnmatchedGen%sTable'%name,
        EECRes4TableProducer.clone(
            EECs = "Gen%s:unmatchedGen"%name,
            name =  "UnmatchedGen%s"%name,
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

    setattr(process, "%sGenTask"%name,
        cms.Task(
            getattr(process, 'Gen%s'%name),
            getattr(process, 'Gen%sTable'%name),
            getattr(process, 'Transfer%sTable'%name),
            getattr(process, 'UnmatchedGen%sTable'%name),
            getattr(process, 'UntransferedGen%sTable'%name),
            getattr(process, 'UntransferedReco%sTable'%name),
        )
    )
    process.schedule.associate(getattr(process, '%sGenTask'%name))

    return process
