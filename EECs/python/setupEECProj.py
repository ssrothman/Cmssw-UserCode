import FWCore.ParameterSet.Config as cms

from SRothman.EECs.EECProjProducer_cfi import *
from SRothman.EECs.EECProjMatchedProducer_cfi import *
from SRothman.EECs.EECProjTransferProducer_cfi import *
from SRothman.EECs.EECProjTableProducer_cfi import *
from SRothman.EECs.EECProjTransferTableProducer_cfi import *
from SRothman.EECs.projcalculator_cfi import *

def setupEECProj_data(process,
                      name,
                      recojets,
                      flags=['Preselection', 'OverlapVeto'],
                      resulttype='Unbinned',
                      verbose=0):

    if resulttype=='Unbinned':
        theProducer = EECProjUnbinnedProducer
        tableProducer = EECProjUnbinnedTableProducer
    elif resulttype=='Vector':
        theProducer = EECProjVectorProducer
        tableProducer = EECProjVectorTableProducer

        process.nanoMetadata.strings.RbinsReco = cms.string(repr(projcalculator_reco.bins.R.value()))
    elif resulttype=='Array':
        theProducer = EECProjArrayProducer
        tableProducer = EECProjArrayTableProducer

        process.nanoMetadata.strings.RbinRreco = cms.string(repr(projcalculator_reco.bins.R.value()))
    else:
        raise ValueError("Unknown result type %s"%resulttype)
    
    flags = [recojets + flag for flag in flags]

    setattr(process, name,
        theProducer.clone(
            jets = recojets,
            flags = flags,
            calculator = projcalculator_reco,
        )
    )
    setattr(process, "Reco%sTable"%name,
        tableProducer.clone(
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

def setupEECProj_MC(process, 
              name, genMatch,
              genjets, recojets,
              flags=['Preselection', 'OverlapVeto'],
              resulttype = 'Unbinned',
              verbose=0):


    if resulttype=='Unbinned':
        theProducer = EECProjMatchedUnbinnedProducer
        tableProducer = EECProjUnbinnedTableProducer
        transferProducer = EECProjTransferUnbinnedProducer
        transferTableProducer = EECProjTransferUnbinnedTableProducer
    elif resulttype=='Vector':
        theProducer = EECProjMatchedVectorProducer
        tableProducer = EECProjVectorTableProducer
        transferProducer = EECProjTransferVectorProducer
        transferTableProducer = EECProjTransferVectorTableProducer

        process.nanoMetadata.strings.RbinsReco = cms.string(repr(projtransfercalculator.bins_reco.R.value()))
        process.nanoMetadata.strings.RbinsGen  = cms.string(repr(projtransfercalculator.bins_gen.R.value()))
    elif resulttype=='Array':
        theProducer = EECProjMatchedArrayProducer
        tableProducer = EECProjArrayTableProducer
        transferProducer = EECProjTransferArrayProducer
        transferTableProducer = EECProjTransferArrayTableProducer

        process.nanoMetadata.strings.RbinsReco = cms.string(repr(projtransfercalculator.bins_reco.R.value()))
        process.nanoMetadata.strings.RbinsGen  = cms.string(repr(projtransfercalculator.bins_gen.R.value()))

    flags = [recojets + flag for flag in flags]

    setattr(process, name,
        theProducer.clone(
            jets = recojets,
            matches = genMatch,
            flags = flags,
            calculator = projcalculator_reco,
        )
    )
    setattr(process, "Reco%sTable"%name,
        tableProducer.clone(
            EECs = "%s:reco"%name,
            name = "Reco%s"%name,
        )
    )
    setattr(process, "UnmatchedReco%sTable"%name,
        tableProducer.clone(
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
        transferProducer.clone(
            genJets = genjets,
            recoJets = recojets,
            matches = genMatch,
            flags = flags,
            calculator = projtransfercalculator
        )
    )
    setattr(process, 'Gen%sTable'%name,
        tableProducer.clone(
            EECs = "Gen%s:gen"%name,
            name =  "Gen%s"%name,
        )
    )
    setattr(process,'Transfer%sTable'%name,
        transferTableProducer.clone(
            EECTransfer = "Gen%s:transfer"%name,
            name = "%sTransfer"%name,
        )
    )
    setattr(process, 'UnmatchedGen%sTable'%name,
        tableProducer.clone(
            EECs = "Gen%s:unmatchedGen"%name,
            name =  "UnmatchedGen%s"%name,
        )
    )

    setattr(process, "%sGenTask"%name,
        cms.Task(
            getattr(process, 'Gen%s'%name),
            getattr(process, 'Gen%sTable'%name),
            getattr(process, 'Transfer%sTable'%name),
            getattr(process, 'UnmatchedGen%sTable'%name),
        )
    )
    process.schedule.associate(getattr(process, '%sGenTask'%name))

    return process


