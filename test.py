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
    input = cms.untracked.int32(-1)
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

minPartPt = 0.0

process.EMDFlow = cms.EDProducer("EMDFlowProducer",
    jets = cms.InputTag("MyGoodJets"),
    genJets = cms.InputTag("MyGenJets"),
    dR2cut = cms.double(0.2*0.2),
    minPartPt = cms.double(minPartPt),
    mode = cms.string("match"),
    partDR2cut = cms.double(0.05*0.05)
)

process.EMDseq = cms.Sequence(process.EMDFlow)

process.EEC2 = cms.EDProducer("PatProjectedEECProducer",
    jets = cms.InputTag("MyGoodJets"),
    order = cms.uint32(2),
    p1 = cms.uint32(1),
    p2 = cms.uint32(1),
    verbose = cms.uint32(0),
    minPartPt = cms.double(minPartPt),
)

process.genEEC2 = cms.EDProducer("GenProjectedEECProducer",
    jets = cms.InputTag("MyGenJets"),
    order = cms.uint32(2),
    p1 = cms.uint32(1),
    p2 = cms.uint32(1),
    verbose = cms.uint32(0),
    minPartPt = cms.double(minPartPt),
)

process.EEC2Transfer = cms.EDProducer("PatProjectedEECTransferProducer",
    jets = cms.InputTag("MyGoodJets"),
    genJets = cms.InputTag("MyGenJets"),
    nDR = cms.uint32(1),
    EECs = cms.InputTag("EEC2"),
    genEECs = cms.InputTag("genEEC2"),
    flows = cms.InputTag("EMDFlow"),
    mode = cms.string("tuples"),
)

process.EECseq = cms.Sequence(process.EEC2 + process.genEEC2 + process.EEC2Transfer)

process.analyzer = cms.EDAnalyzer("TransferAnalyzer",
    src = cms.InputTag("EEC2Transfer"),
    genEEC = cms.InputTag("genEEC2"),
    recoEEC = cms.InputTag("EEC2"),
    recoJets = cms.InputTag("MyGoodJets"),
    genJets = cms.InputTag("MyGenJets"),
    minpt = cms.double(10.0),
    nBins = cms.uint32(20),
)

process.aseq = cms.Sequence(process.analyzer)

process.MyGoodMuons = cms.EDFilter("PATMuonSelector",
    src = cms.InputTag('slimmedMuons'),
    cut = cms.string("pt > 20 && abs(eta) < 2.5 && CutBasedIdTight && PFIsoTight"),
    filter = cms.bool(True),
)

process.MyZs = cms.EDProducer("CandViewShallowCloneCombiner",
    checkCharge = cms.bool(True),
    cut = cms.string("charge == 0 & mass > 80 & mass < 100"),
    decay = cms.string("MyGoodMuons@+ MyGoodMuons@-")
)

process.ZFilter = cms.EDFilter("CandViewCountFilter",
    src = cms.InputTag("MyZs"),
    minNumber = cms.uint32(1)
)

process.Zseq = cms.Sequence(process.MyGoodMuons + process.MyZs + process.ZFilter)

process.MyGoodJets = cms.EDFilter("PATJetSelector",
    src = cms.InputTag("selectedPatJetsAK4PFPuppi"),
    cut = cms.string("pt>30 && abs(eta) < 2.1 && userInt('tightId')==1 && numberOfDaughters>1 && muonMultiplicity==0"),
    filter = cms.bool(True),
)

process.MyGenJets = cms.EDFilter("GenJetSelector",
    src = cms.InputTag("ak4GenJetsNoNu"),
    cut = cms.string("pt>20 && abs(eta) < 2.4 && numberOfDaughters>1"),
    filter = cms.bool(False)
)

process.Jetseq = cms.Sequence(process.MyGoodJets + process.MyGenJets)

process.EEC_PATH = cms.Path(process.Jetseq + process.Zseq + process.EMDseq + process.EECseq + process.aseq)

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

#process.schedule = cms.Schedule(*[process.EEC_PATH, process.nanoAOD_step,], tasks=[process.customizedPFCandsTask, process.patAlgosToolsTask])
process.schedule = cms.Schedule(*[process.EEC_PATH, process.nanoAOD_step, process.endjob_step, process.NANOAODSIMoutput_step ], tasks=[process.customizedPFCandsTask, process.patAlgosToolsTask])
