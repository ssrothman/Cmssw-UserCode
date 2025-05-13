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
                      resulttype='Unbinned',
                      verbose=0):

    if resulttype=='Unbinned':
        theProducer = EECRes4UnbinnedProducer
        tableProducer = EECRes4UnbinnedTableProducer
    elif resulttype=='Vector':
        theProducer = EECRes4VectorProducer
        tableProducer = EECRes4VectorTableProducer

        process.nanoMetadata.strings.RbinsReco         = cms.string(repr(EECRes4VectorProducer.calculator.bins.R.value()))
        process.nanoMetadata.strings.rDIPOLEbinsReco   = cms.string(repr(EECRes4VectorProducer.calculator.bins.r_dipole.value()))
        process.nanoMetadata.strings.cDIPOLEbinsReco   = cms.string(repr(EECRes4VectorProducer.calculator.bins.c_dipole.value()))
        process.nanoMetadata.strings.rTEEbinsReco      = cms.string(repr(EECRes4VectorProducer.calculator.bins.r_tee.value()))
        process.nanoMetadata.strings.cTEEbinsReco      = cms.string(repr(EECRes4VectorProducer.calculator.bins.c_tee.value()))
        process.nanoMetadata.strings.rTRIANGLEbinsReco = cms.string(repr(EECRes4VectorProducer.calculator.bins.r_triangle.value()))
        process.nanoMetadata.strings.cTRIANGLEbinsReco = cms.string(repr(EECRes4VectorProducer.calculator.bins.c_triangle.value()))
    elif resulttype=='Array':
        theProducer = EECRes4ArrayProducer
        tableProducer = EECRes4ArrayTableProducer

        process.nanoMetadata.strings.RbinsReco         = cms.string(repr(EECRes4ArrayProducer.calculator.bins.R.value()))
        process.nanoMetadata.strings.rDIPOLEbinsReco   = cms.string(repr(EECRes4ArrayProducer.calculator.bins.r_dipole.value()))
        process.nanoMetadata.strings.cDIPOLEbinsReco   = cms.string(repr(EECRes4ArrayProducer.calculator.bins.c_dipole.value()))
        process.nanoMetadata.strings.rTEEbinsReco      = cms.string(repr(EECRes4ArrayProducer.calculator.bins.r_tee.value()))
        process.nanoMetadata.strings.cTEEbinsReco      = cms.string(repr(EECRes4ArrayProducer.calculator.bins.c_tee.value()))
        process.nanoMetadata.strings.rTRIANGLEbinsReco = cms.string(repr(EECRes4ArrayProducer.calculator.bins.r_triangle.value()))
        process.nanoMetadata.strings.cTRIANGLEbinsReco = cms.string(repr(EECRes4ArrayProducer.calculator.bins.c_triangle.value()))
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

def setupEECRes4_MC(process, 
              name, genMatch,
              genjets, recojets,
              flags=['Preselection', 'OverlapVeto'],
              resulttype = 'Unbinned',
              verbose=0):

    if resulttype=='Unbinned':
        theProducer = EECRes4MatchedUnbinnedProducer
        tableProducer = EECRes4UnbinnedTableProducer
        transferProducer = EECRes4TransferUnbinnedProducer
        transferTableProducer = EECRes4TransferUnbinnedTableProducer
    elif resulttype=='Vector':
        theProducer = EECRes4MatchedVectorProducer
        tableProducer = EECRes4VectorTableProducer
        transferProducer = EECRes4TransferVectorProducer
        transferTableProducer = EECRes4TransferVectorTableProducer

        process.nanoMetadata.strings.RbinsReco         = cms.string(repr(EECRes4VectorProducer.calculator.bins.R.value()))
        process.nanoMetadata.strings.rDIPOLEbinsReco   = cms.string(repr(EECRes4VectorProducer.calculator.bins.r_dipole.value()))
        process.nanoMetadata.strings.cDIPOLEbinsReco   = cms.string(repr(EECRes4VectorProducer.calculator.bins.c_dipole.value()))
        process.nanoMetadata.strings.rTEEbinsReco      = cms.string(repr(EECRes4VectorProducer.calculator.bins.r_tee.value()))
        process.nanoMetadata.strings.cTEEbinsReco      = cms.string(repr(EECRes4VectorProducer.calculator.bins.c_tee.value()))
        process.nanoMetadata.strings.rTRIANGLEbinsReco = cms.string(repr(EECRes4VectorProducer.calculator.bins.r_triangle.value()))
        process.nanoMetadata.strings.cTRIANGLEbinsReco = cms.string(repr(EECRes4VectorProducer.calculator.bins.c_triangle.value()))

        process.nanoMetadata.strings.RbinsGen         = cms.string(repr(EECRes4VectorProducer.calculator.bins.R.value()))
        process.nanoMetadata.strings.rDIPOLEbinsGen   = cms.string(repr(EECRes4VectorProducer.calculator.bins.r_dipole.value()))
        process.nanoMetadata.strings.cDIPOLEbinsGen   = cms.string(repr(EECRes4VectorProducer.calculator.bins.c_dipole.value()))
        process.nanoMetadata.strings.rTEEbinsGen      = cms.string(repr(EECRes4VectorProducer.calculator.bins.r_tee.value()))
        process.nanoMetadata.strings.cTEEbinsGen      = cms.string(repr(EECRes4VectorProducer.calculator.bins.c_tee.value()))
        process.nanoMetadata.strings.rTRIANGLEbinsGen = cms.string(repr(EECRes4VectorProducer.calculator.bins.r_triangle.value()))
        process.nanoMetadata.strings.cTRIANGLEbinsGen = cms.string(repr(EECRes4VectorProducer.calculator.bins.c_triangle.value()))
    elif resulttype=='Array':
        theProducer = EECRes4MatchedArrayProducer
        tableProducer = EECRes4ArrayTableProducer
        transferProducer = EECRes4TransferArrayProducer
        transferTableProducer = EECRes4TransferArrayTableProducer

        process.nanoMetadata.strings.RbinsReco         = cms.string(repr(EECRes4ArrayProducer.calculator.bins.R.value()))
        process.nanoMetadata.strings.rDIPOLEbinsReco   = cms.string(repr(EECRes4ArrayProducer.calculator.bins.r_dipole.value()))
        process.nanoMetadata.strings.cDIPOLEbinsReco   = cms.string(repr(EECRes4ArrayProducer.calculator.bins.c_dipole.value()))
        process.nanoMetadata.strings.rTEEbinsReco      = cms.string(repr(EECRes4ArrayProducer.calculator.bins.r_tee.value()))
        process.nanoMetadata.strings.cTEEbinsReco      = cms.string(repr(EECRes4ArrayProducer.calculator.bins.c_tee.value()))
        process.nanoMetadata.strings.rTRIANGLEbinsReco = cms.string(repr(EECRes4ArrayProducer.calculator.bins.r_triangle.value()))
        process.nanoMetadata.strings.cTRIANGLEbinsReco = cms.string(repr(EECRes4ArrayProducer.calculator.bins.c_triangle.value()))

        process.nanoMetadata.strings.RbinsGen         = cms.string(repr(EECRes4ArrayProducer.calculator.bins.R.value()))
        process.nanoMetadata.strings.rDIPOLEbinsGen   = cms.string(repr(EECRes4ArrayProducer.calculator.bins.r_dipole.value()))
        process.nanoMetadata.strings.cDIPOLEbinsGen   = cms.string(repr(EECRes4ArrayProducer.calculator.bins.c_dipole.value()))
        process.nanoMetadata.strings.rTEEbinsGen      = cms.string(repr(EECRes4ArrayProducer.calculator.bins.r_tee.value()))
        process.nanoMetadata.strings.cTEEbinsGen      = cms.string(repr(EECRes4ArrayProducer.calculator.bins.c_tee.value()))
        process.nanoMetadata.strings.rTRIANGLEbinsGen = cms.string(repr(EECRes4ArrayProducer.calculator.bins.r_triangle.value()))
        process.nanoMetadata.strings.cTRIANGLEbinsGen = cms.string(repr(EECRes4ArrayProducer.calculator.bins.c_triangle.value()))

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
    setattr(process, 'UntransferedGen%sTable'%name,
        tableProducer.clone(
            EECs = "Gen%s:untransferedGen"%name,
            name =  "UntransferedGen%s"%name,
        )
    )
    setattr(process, 'UntransferedReco%sTable'%name,
        tableProducer.clone(
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
