import FWCore.ParameterSet.Config as cms

from SRothman.EECs.EECRes3Producer_cfi import *
from SRothman.EECs.EECRes3MatchedProducer_cfi import *
from SRothman.EECs.EECRes3TransferProducer_cfi import *
from SRothman.EECs.EECRes3TableProducer_cfi import *
from SRothman.EECs.EECRes3TransferTableProducer_cfi import *

def setupEECRes3_data(process,
                      name,
                      recojets,
                      flags=['Preselection', 'OverlapVeto'],
                      resulttype='Unbinned',
                      verbose=0):

    if resulttype=='Unbinned':
        theProducer = EECRes3UnbinnedProducer
        tableProducer = EECRes3UnbinnedTableProducer
    elif resulttype=='Vector':
        theProducer = EECRes3VectorProducer
        tableProducer = EECRes3VectorTableProducer

        process.nanoMetadata.strings.RbinsReco = cms.string(repr(EECRes3VectorProducer.calculator.bins.R.value()))
        process.nanoMetadata.strings.rbinsReco = cms.string(repr(EECRes3VectorProducer.calculator.bins.r.value()))
        process.nanoMetadata.strings.cbinsReco = cms.string(repr(EECRes3VectorProducer.calculator.bins.c.value()))
    elif resulttype=='Array':
        theProducer = EECRes3ArrayProducer
        tableProducer = EECRes3ArrayTableProducer

        process.nanoMetadata.strings.RbinRreco = cms.string(repr(EECRes3ArrayProducer.calculator.bins.R.value()))
        process.nanoMetadata.strings.rbinRreco = cms.string(repr(EECRes3ArrayProducer.calculator.bins.r.value()))
        process.nanoMetadata.strings.cbinRreco = cms.string(repr(EECRes3ArrayProducer.calculator.bins.c.value()))
    else:
        raise ValueError("Unknown result type %s"%resulttype)
    
    flags = [recojets + flag for flag in flags]

    setattr(process, name,
        theProducer.clone(
            jets = recojets,
            flags = flags,
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

def setupEECRes3_MC(process, 
              name, genMatch,
              genjets, recojets,
              flags=['Preselection', 'OverlapVeto'],
              resulttype = 'Unbinned',
              verbose=0):

    if resulttype=='Unbinned':
        theProducer = EECRes3MatchedUnbinnedProducer
        tableProducer = EECRes3UnbinnedTableProducer
        transferProducer = EECRes3TransferUnbinnedProducer
        transferTableProducer = EECRes3TransferUnbinnedTableProducer
    elif resulttype=='Vector':
        theProducer = EECRes3MatchedVectorProducer
        tableProducer = EECRes3VectorTableProducer
        transferProducer = EECRes3TransferVectorProducer
        transferTableProducer = EECRes3TransferVectorTableProducer

        process.nanoMetadata.strings.RbinsReco = cms.string(repr(EECRes3TransferVectorProducer.calculator.bins_reco.R.value()))
        process.nanoMetadata.strings.RbinsGen = cms.string(repr(EECRes3TransferVectorProducer.calculator.bins_gen.R.value()))
        process.nanoMetadata.strings.rbinsReco = cms.string(repr(EECRes3TransferVectorProducer.calculator.bins_reco.r.value()))
        process.nanoMetadata.strings.rbinsGen = cms.string(repr(EECRes3TransferVectorProducer.calculator.bins_gen.r.value()))
        process.nanoMetadata.strings.cbinsReco = cms.string(repr(EECRes3TransferVectorProducer.calculator.bins_reco.c.value()))
        process.nanoMetadata.strings.cbinsGen = cms.string(repr(EECRes3TransferVectorProducer.calculator.bins_gen.c.value()))
    elif resulttype=='Array':
        theProducer = EECRes3MatchedArrayProducer
        tableProducer = EECRes3ArrayTableProducer
        transferProducer = EECRes3TransferArrayProducer
        transferTableProducer = EECRes3TransferArrayTableProducer

        process.nanoMetadata.strings.RbinsReco = cms.string(repr(EECRes3TransferArrayProducer.calculator.bins_reco.R.value()))
        process.nanoMetadata.strings.RbinsGen = cms.string(repr(EECRes3TransferArrayProducer.calculator.bins_gen.R.value()))
        process.nanoMetadata.strings.rbinsReco = cms.string(repr(EECRes3TransferArrayProducer.calculator.bins_reco.r.value()))
        process.nanoMetadata.strings.rbinsGen = cms.string(repr(EECRes3TransferArrayProducer.calculator.bins_gen.r.value()))
        process.nanoMetadata.strings.cbinsReco = cms.string(repr(EECRes3TransferArrayProducer.calculator.bins_reco.c.value()))
        process.nanoMetadata.strings.cbinsGen = cms.string(repr(EECRes3TransferArrayProducer.calculator.bins_gen.c.value()))

    flags = [recojets + flag for flag in flags]

    setattr(process, name,
        theProducer.clone(
            jets = recojets,
            matches = genMatch,
            flags = flags
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

