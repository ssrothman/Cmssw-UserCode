# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: NANO -s NANO --mc --conditions 106x_mc2017_realistic_v9-v2 --era Run2_2017,run2_nanoAOD_106Xv2 --eventcontent NANOAODSIM --datatier NANOAODSIM --customise_commands=process.add_(cms.Service('InitRootHandlers', EnableIMT = cms.untracked.bool(False)));process.MessageLogger.cerr.FwkReport.reportEvery=1000 -n -1 --no_exec
import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run2_2018_cff import Run2_2018
from Configuration.Eras.Modifier_run2_nanoAOD_106Xv2_cff import run2_nanoAOD_106Xv2

from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing('analysis')
options.register('index', 
                 -1,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.int,
                 "Job index to process")
options.register('inputjson',
                 '',
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "JSON file with index to files mapping")
options.register('outputdir',
                 '',
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Output directory")
options.parseArguments()

if options.index < 0 or options.inputjson == '' or options.outputdir == '':
    print('Please provide an index, inputjson, and outputdir')
    exit(1)

process = cms.Process('NANO',Run2_2018,run2_nanoAOD_106Xv2)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# Input source
import json
with open(options.inputjson, 'r') as f:
    filejson = json.load(f)

filelist = filejson[str(options.index)]
cleanlist = []
for file in filelist:
    cleanlist.append(str(file))

print(str(filelist[0]))
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(cleanlist),
    secondaryFileNames = cms.untracked.vstring(),
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
    fileName = cms.untracked.string('%s/NANO_%d.root'%(options.outputdir,options.index)),
    outputCommands = process.NANOAODSIMEventContent.outputCommands,
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('main_path')
    ),
    #autoFlush = cms.untracked.int32(1)
)

process.DroppedEventsOutput = cms.OutputModule("NanoAODOutputModule",
    compressionAlgorithm = cms.untracked.string('LZMA'),
    compressionLevel = cms.untracked.int32(9),
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('NANOAODSIM'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('%s/dropped_%d.root'%(options.outputdir,options.index)),
    outputCommands = cms.untracked.vstring(
        'drop *',
        'keep nanoaodFlatTable_genTable_*_*',
        'keep nanoaodFlatTable_genWeightsTable_*_*',
        'keep nanoaodFlatTable_lheInfoTable_*_*',),
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('!main_path')
    ),
    #autoFlush = cms.untracked.int32(1)
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_upgrade2018_realistic_v16_L1v1', '')

# Path and EndPath definitions
from SRothman.Analysis.ZDecayEventSelectionProducer_cfi import *
process.GenZDecay = ZDecayEventSelectionProducer.clone()
process.main_path = cms.Path(process.GenZDecay)

#process.nanoAOD_step = cms.Path(process.nanoSequenceMC)
#process.endjob_step = cms.EndPath(process.endOfProcess)
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)
process.DroppedEventsOutput_step = cms.EndPath(process.DroppedEventsOutput)

# Schedule definition
process.schedule = cms.Schedule(process.main_path, process.NANOAODSIMoutput_step, process.DroppedEventsOutput_step)
#from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
#associatePatAlgosToolsTask(process)

#Setup FWK for multithreaded
process.options.numberOfThreads=cms.untracked.uint32(4)
process.options.numberOfStreams=cms.untracked.uint32(4)
process.options.numberOfConcurrentLuminosityBlocks=cms.untracked.uint32(1)

# customisation of the process.

# Automatic addition of the customisation function from PhysicsTools.NanoAOD.nano_cff
#from PhysicsTools.NanoAOD.nano_cff import nanoAOD_customizeMC 

#call to customisation function nanoAOD_customizeMC imported from PhysicsTools.NanoAOD.nano_cff
#process = nanoAOD_customizeMC(process)

#from SRothman.Analysis.setupAnalysis import *
#process = setupAnalysis(process, 
#                        ak8=True, 
#                        doCharged=True,
#                        doNominal=False, 
#                        addNaive=False, 
#                        isMC=True, 
#                        addFullEvent=False, 
#                        addRandomControl=True,
#                        addZControl=False,
#                        verbose=1)
#from SRothman.Analysis.setupZMuMu import setupGenZDecay
from SRothman.Analysis.addParticlesTable import addParticlesTable
from SRothman.CustomJets.setupAK8Jets import setupAK8Jets
from SRothman.CustomJets.setupSimonJets import setupSimonJets
from SRothman.EECs.setupEECs import setupEECs
from SRothman.Analysis.setupGenTables import setupGenTables
process = setupGenTables(process)
process = addParticlesTable(process, 
                            'GenZDecay:daughters', 
                            'GenZDecayParticles', 
                             singleton=False)
process = addParticlesTable(process, 
                            'GenZDecay:Z', 
                            'GenZ', 
                            singleton=True)
process = setupAK8Jets(process,
                       isMC=True,
                       skipJTB=True,
                       genOnly=True,
                       genParticles='genParticles',
                       extracut='muonEnergy/(pt*cosh(eta)) < 0.6')
process = setupSimonJets(process,
                         jets = '',
                         genjets = 'selectedGenJetsAK8',
                         CHSjets = '',
                         chargedOnly = False,
                         eventSelection = '',
                         name = 'SimonJets',
                         ak8 = True,
                         isMC = True,
                         genOnly = True)
process = setupSimonJets(process,
                         jets = '',
                         genjets = 'selectedGenJetsAK8',
                         CHSjets = '',
                         chargedOnly = True,
                         eventSelection = '',
                         name = 'ChargedSimonJets',
                         ak8 = True,
                         isMC = True,
                         genOnly = True)
process = setupEECs(process,
                    name = 'EECs',
                    genMatch = '',
                    genjets = '',
                    recojets = 'GenSimonJets',
                    verbose = 0,
                    isMC = False)
process = setupEECs(process,
                    name = 'ChargedEECs',
                    genMatch = '',
                    genjets = '',
                    recojets = 'GenChargedSimonJets',
                    verbose = 0,
                    isMC = False)

# End of customisation functions

# Customisation from command line

process.add_(cms.Service('InitRootHandlers', EnableIMT = cms.untracked.bool(False)));
process.MessageLogger.cerr.FwkReport.reportEvery=1
# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
