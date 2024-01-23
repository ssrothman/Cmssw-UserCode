# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: NANO -s NANO --mc --conditions 106x_mc2017_realistic_v9-v2 --era Run2_2017,run2_nanoAOD_106Xv2 --eventcontent NANOAODSIM --datatier NANOAODSIM --customise_commands=process.add_(cms.Service('InitRootHandlers', EnableIMT = cms.untracked.bool(False)));process.MessageLogger.cerr.FwkReport.reportEvery=1000 -n -1 --no_exec
import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run2_2018_cff import Run2_2018
from Configuration.Eras.Modifier_run2_nanoAOD_106Xv2_cff import run2_nanoAOD_106Xv2

process = cms.Process('NANO',Run2_2018,run2_nanoAOD_106Xv2)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('PhysicsTools.NanoAOD.nano_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(50)
)

# Input source
process.source = cms.Source("PoolSource",
    #fileNames = cms.untracked.vstring('/store/mc/RunIISummer20UL17MiniAODv2/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v2/120000/005B6A7C-B0B1-A745-879B-017FE7933B77.root'),
    fileNames = cms.untracked.vstring('/store/mc/RunIISummer20UL18MiniAODv2/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/106X_upgrade2018_realistic_v16_L1v1-v2/120000/001C8DDF-599C-5E45-BF2C-76F887C9ADE9.root'),
    secondaryFileNames = cms.untracked.vstring(),
    #eventsToProcess = cms.untracked.VEventRange(cms.EventRange(1, 129169955, 1, 129169957))
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('NANO nevts:-1'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.NANOAODSIMoutput = cms.OutputModule("NanoAODOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(9),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('NANOAODSIM'),
        filterName = cms.untracked.string('')
    ),
    #fileName = cms.untracked.string('/data/submit/cms/store/user/srothman/NANO_NANO_10k_3.root'),
    #fileName = cms.untracked.string('~/cmsdata/NANO_NANO.root'),
    fileName = cms.untracked.string('NANO_miniAOD.root'),
    outputCommands = process.NANOAODSIMEventContent.outputCommands,
    #autoFlush = cms.untracked.int32(1)
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_upgrade2018_realistic_v16_L1v1', '')

# Path and EndPath definitions
process.nanoAOD_step = cms.Path(process.nanoSequenceMC)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)

# Schedule definition
process.schedule = cms.Schedule(process.nanoAOD_step,process.endjob_step,process.NANOAODSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(1)
process.options.numberOfStreams=cms.untracked.uint32(1)
process.options.numberOfConcurrentLuminosityBlocks=cms.untracked.uint32(1)

# customisation of the process.

# Automatic addition of the customisation function from PhysicsTools.NanoAOD.nano_cff
from PhysicsTools.NanoAOD.nano_cff import nanoAOD_customizeMC 

#call to customisation function nanoAOD_customizeMC imported from PhysicsTools.NanoAOD.nano_cff
process = nanoAOD_customizeMC(process)

from SRothman.CustomJets.SimonJetTableProducer_cfi import *

process.patJetsReapplyJECPuppi.tagInfoSources += ['slimmedJetsPuppi:tagInfos']
process.patJetsReapplyJECPuppi.addBTagInfo = True
process.patJetsReapplyJECPuppi.addTagInfos = True

process.jetIdLepVetoPuppi = cms.EDProducer("PatJetIDValueMapProducer",
    filterParams = cms.PSet(
        quality = cms.string("TIGHTLEPVETO"),
        version = cms.string("RUN2ULPUPPI"),
    ),
    src = cms.InputTag('patJetsReapplyJECPuppi')
)

from RecoJets.JetProducers.PileupJetID_cfi import _chsalgos_106X_UL18
process.pileupJetIdPuppi = process.pileupJetId.clone( 
    jets=cms.InputTag('patJetsReapplyJECPuppi'),
    inputIsCorrected=True,
    applyJec=False,
    vertexes=cms.InputTag("offlineSlimmedPrimaryVertices"),
    algos = cms.VPSet(_chsalgos_106X_UL18),
)

process.updatedJetsPuppi = cms.EDProducer("PATJetUserDataEmbedder",
    src = cms.InputTag("patJetsReapplyJECPuppi"),
    userInts = cms.PSet(
        puID = cms.InputTag("pileupJetIdPuppi:fullId"),
        jetIdLepVeto = cms.InputTag("jetIdLepVetoPuppi"),
    ) 
)

process.extraJetTask = cms.Task(
    process.pileupJetIdPuppi,
    process.jetIdLepVetoPuppi,
    process.updatedJetsPuppi,
)
process.schedule.associate(process.extraJetTask)

process.PatJetTestProducer = cms.EDProducer("PatJetTestProducer",
    verbose = cms.int32(1),
    src = cms.InputTag("updatedJetsPuppi"),
    packedCandSrc = cms.InputTag("packedPFCandidates")
)
process.PatJetTestTable = SimonJetTableProducer.clone(
    src = 'PatJetTestProducer',
    name = 'PatJetTest',
    verbose=0,
    isGen = False
)
process.PatJetTask = cms.Task(
    process.PatJetTestProducer,
    process.PatJetTestTable
)
process.schedule.associate(process.PatJetTask)
# End of customisation functions

# Customisation from command line

process.add_(cms.Service('InitRootHandlers', EnableIMT = cms.untracked.bool(False)));
process.MessageLogger.cerr.FwkReport.reportEvery=1
# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
