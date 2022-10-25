# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: nano_mc_2017_UL --mc --eventcontent NANOAODSIM --datatier NANOAODSIM --step NANO --conditions 106X_mc2017_realistic_v8 --era Run2_2017,run2_nanoAOD_106Xv1 --customise_commands=process.add_(cms.Service('InitRootHandlers', EnableIMT = cms.untracked.bool(False))) --nThreads 4 -n 100 --filein /store/mc/RunIISummer19UL17MiniAOD/QCD_Pt_1400to1800_TuneCP5_13TeV_pythia8/MINIAODSIM/106X_mc2017_realistic_v6-v2/100000/BFAAC85A-F5C5-8843-8D2A-76A9E873E24B.root --fileout file:nano_mc2017.root --customise PhysicsTools/PFNano/pfnano_cff.PFnano_customizeMC --no_exec
import FWCore.ParameterSet.Config as cms

from Configuration.Eras.Era_Run2_2017_cff import Run2_2017
from Configuration.Eras.Modifier_run2_nanoAOD_106Xv1_cff import run2_nanoAOD_106Xv1

process = cms.Process('NANO',Run2_2017,run2_nanoAOD_106Xv1)

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
    input = cms.untracked.int32(100)
)

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:DYJetsToLL_miniAOD.root'),
    secondaryFileNames = cms.untracked.vstring()
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('nano_mc_2017_UL nevts:100'),
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
    fileName = cms.untracked.string('file:nano_mc2017.root'),
    outputCommands = process.NANOAODSIMEventContent.outputCommands
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_mc2017_realistic_v8', '')

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
from SRothman.MyJets.myjets_cff import setupCustomizedJetToolbox, addPFCands
process = setupCustomizedJetToolbox(process, runOnMC=True, PU="Puppi")
process = addPFCands(process, runOnMC=True, jetsName="selectedPatJetsAK4PFPuppi")

process.EMDFlow = cms.EDProducer("EMDFlowProducer",
    jets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    genJets = cms.InputTag("ak4GenJetsNoNu"),
    dR2cut = cms.double(0.2*0.2),
    minPartPt = cms.double(0.0),
    mode = cms.string("Ewt"),
)

process.EMDFlowTable = cms.EDProducer("EMDFlowTableProducer",
    flow = cms.InputTag("EMDFlow")
)

process.EMDTask = cms.Task(process.EMDFlow)

process.schedule.associate(process.EMDTask)

process.EEC2 = cms.EDProducer("PatProjectedEECProducer",
    jets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    order = cms.uint32(2),
    minJetPt = cms.double(30),
    muons = cms.InputTag('finalMuons'),
    p1 = cms.uint32(1),
    p2 = cms.uint32(1),
    verbose = cms.uint32(0),
    minPartPt = cms.double(0.0),
)

process.genEEC2 = cms.EDProducer("GenProjectedEECProducer",
    jets = cms.InputTag("ak4GenJetsNoNu"),
    order = cms.uint32(2),
    minJetPt = cms.double(10),
    muons = cms.InputTag('finalMuons'),
    p1 = cms.uint32(1),
    p2 = cms.uint32(1),
    verbose = cms.uint32(0),
    minPartPt = cms.double(0.0),
)

process.EEC2Table = cms.EDProducer("PatProjectedEECTableProducer",
    jets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    nDR = cms.uint32(1),
    name = cms.string("EEC2"),
    EECs = cms.InputTag("EEC2")
)

process.genEEC2Table = cms.EDProducer("GenProjectedEECTableProducer",
    jets = cms.InputTag("ak4GenJetsNoNu"),
    nDR = cms.uint32(1),
    name = cms.string("genEEC2"),
    EECs = cms.InputTag("genEEC2")
)

process.EEC2TransferTable = cms.EDProducer("PatProjectedEECTransferProducer",
    jets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    genJets = cms.InputTag("ak4GenJetsNoNu"),
    nDR = cms.uint32(1),
    EECs = cms.InputTag("EEC2"),
    genEECs = cms.InputTag("genEEC2"),
    flows = cms.InputTag("EMDFlow"),
    mode = cms.string("AF"),
)

process.EECTask = cms.Task(process.EEC2, process.genEEC2, process.EEC2TransferTable)
process.schedule.associate(process.EECTask)

process.EEC3 = cms.EDProducer("PatProjectedEECProducer",
    jets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    order = cms.uint32(3),
    minJetPt = cms.double(30),
    muons = cms.InputTag('finalMuons'),
    p1 = cms.uint32(1),
    p2 = cms.uint32(1),
    verbose = cms.uint32(0)
)

process.genEEC3 = cms.EDProducer("GenProjectedEECProducer",
    jets = cms.InputTag("ak4GenJetsNoNu"),
    order = cms.uint32(3),
    minJetPt = cms.double(10),
    muons = cms.InputTag('finalMuons'),
    p1 = cms.uint32(1),
    p2 = cms.uint32(1),
    verbose = cms.uint32(0)
)

process.EEC3Table = cms.EDProducer("PatProjectedEECTableProducer",
    jets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    nDR = cms.uint32(1),
    name = cms.string("EEC3"),
    EECs = cms.InputTag("EEC3")
)

process.genEEC3Table = cms.EDProducer("GenProjectedEECTableProducer",
    jets = cms.InputTag("ak4GenJetsNoNu"),
    nDR = cms.uint32(1),
    name = cms.string("genEEC3"),
    EECs = cms.InputTag("genEEC3")
)

process.EEC3TransferTable = cms.EDProducer("PatProjectedEECTransferProducer",
    jets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    genJets = cms.InputTag("ak4GenJetsNoNu"),
    nDR = cms.uint32(1),
    EECs = cms.InputTag("EEC3"),
    genEECs = cms.InputTag("genEEC3"),
    flows = cms.InputTag("EMDFlow")
)

#process.EECTask3 = cms.Task(process.EEC3, process.genEEC3, process.EEC3TransferTable)
#process.schedule.associate(process.EECTask3)

process.EEC4 = cms.EDProducer("PatProjectedEECProducer",
    jets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    order = cms.uint32(4),
    minJetPt = cms.double(30),
    muons = cms.InputTag('finalMuons'),
    p1 = cms.uint32(1),
    p2 = cms.uint32(1),
    verbose = cms.uint32(0)
)

process.genEEC4 = cms.EDProducer("GenProjectedEECProducer",
    jets = cms.InputTag("ak4GenJetsNoNu"),
    order = cms.uint32(4),
    minJetPt = cms.double(10),
    muons = cms.InputTag('finalMuons'),
    p1 = cms.uint32(1),
    p2 = cms.uint32(1),
    verbose = cms.uint32(0)
)

process.EEC4Table = cms.EDProducer("PatProjectedEECTableProducer",
    jets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    nDR = cms.uint32(1),
    name = cms.string("EEC4"),
    EECs = cms.InputTag("EEC4")
)

process.genEEC4Table = cms.EDProducer("GenProjectedEECTableProducer",
    jets = cms.InputTag("ak4GenJetsNoNu"),
    nDR = cms.uint32(1),
    name = cms.string("genEEC4"),
    EECs = cms.InputTag("genEEC4")
)

process.EEC4TransferTable = cms.EDProducer("PatProjectedEECTransferProducer",
    jets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    genJets = cms.InputTag("ak4GenJetsNoNu"),
    nDR = cms.uint32(1),
    EECs = cms.InputTag("EEC4"),
    genEECs = cms.InputTag("genEEC4"),
    flows = cms.InputTag("EMDFlow")
)

#process.EECTask4 = cms.Task(process.EEC4, process.genEEC4, process.EEC4TransferTable)
#process.schedule.associate(process.EECTask4)

process.analyzer = cms.EDAnalyzer("TransferAnalyzer",
    src = cms.InputTag("EEC2TransferTable"),
    genEEC = cms.InputTag("genEEC2"),
    recoEEC = cms.InputTag("EEC2"),
    recoJets = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    genJets = cms.InputTag("ak4GenJetsNoNu"),
    minpt = cms.double(50.0),
    nBins = cms.uint32(20),
)
process.aseq = cms.Sequence(process.analyzer)
process.nanoAOD_step += process.aseq

# Automatic addition of the customisation function from PhysicsTools.NanoAOD.nano_cff
from PhysicsTools.NanoAOD.nano_cff import nanoAOD_customizeMC 

#call to customisation function nanoAOD_customizeMC imported from PhysicsTools.NanoAOD.nano_cff
process = nanoAOD_customizeMC(process)

# Automatic addition of the customisation function from PhysicsTools.PFNano.pfnano_cff
#from PhysicsTools.PFNano.pfnano_cff import PFnano_customizeMC 

#call to customisation function PFnano_customizeMC imported from PhysicsTools.PFNano.pfnano_cff
#process = PFnano_customizeMC(process)
process.NANOAODSIMoutput.fakeNameForCrab = cms.untracked.bool(True)  # needed for crab publication
# End of customisation functions

# Customisation from command line

process.add_(cms.Service('InitRootHandlers', EnableIMT = cms.untracked.bool(False)))
# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
