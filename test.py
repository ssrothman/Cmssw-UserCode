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
    outputCommands = cms.untracked.vstring(
      'drop *',
      'keep nanoaodFlatTable_*_Muon_*',
      'keep nanoaodFlatTable_*transfer*_*_*',
      'keep nanoaodFlatTable_*EEC*_*_*',
      'keep nanoaodFlatTable_muonTable_*_*',
      'keep nanoaodFlatTable_*JTBTable_*_*',
      'keep nanoaodFlatTable_genJetsParticleTable_*_*',
      'keep *_TriggerResults_*_HLT',
      'keep *_genWeightsTable_*_*'
    )
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
process = setupCustomizedJetToolbox(process, runOnMC=True, PU="Puppi",
    cut = "pt>30 && abs(eta) < 2.4",
    genJetCut = "pt>20 && abs(eta) < 2.7 && numberOfDaughters>1"
)
process = addPFCands(process, runOnMC=True, jetsName="selectedPatJetsAK4PFPuppi")

minPartPt = 0.0
jets = 'selectedPatJetsAK4PFPuppi'
genJets = 'selectedak4GenJetsNoNu'
muons = "MyGoodMuons"

#produces particle-level transfer 
process.EMDFlow = cms.EDProducer("EMDFlowProducer",
    jets = cms.InputTag(jets),
    genJets = cms.InputTag(genJets),
    dR2cut = cms.double(0.2*0.2),
    minPartPt = cms.double(minPartPt),
    mode = cms.string("Ewt"),
    partDR2cut = cms.double(0.05*0.05)
)

process.EMDTask = cms.Task(process.EMDFlow)

#compute EEC
process.EEC2 = cms.EDProducer("PatProjectedEECProducer",
    jets = cms.InputTag(jets),
    order = cms.uint32(2),
    p1 = cms.uint32(1),
    p2 = cms.uint32(1),
    verbose = cms.uint32(1),
    minPartPt = cms.double(minPartPt),
    muons = cms.InputTag(muons),
    requireZ = cms.bool(True)
)

#compute gen EEC
process.genEEC2 = cms.EDProducer("GenProjectedEECProducer",
    jets = cms.InputTag(genJets),
    order = cms.uint32(2),
    p1 = cms.uint32(1),
    p2 = cms.uint32(1),
    verbose = cms.uint32(0),
    minPartPt = cms.double(minPartPt),
    muons = cms.InputTag(muons),
    requireZ = cms.bool(True)
)

#compute transfer matrix
process.EEC2Transfer = cms.EDProducer("PatProjectedEECTransferProducer",
    jets = cms.InputTag(jets),
    genJets = cms.InputTag(genJets),
    nDR = cms.uint32(1),
    EECs = cms.InputTag("EEC2"),
    genEECs = cms.InputTag("genEEC2"),
    flows = cms.InputTag("EMDFlow"),
    mode = cms.string("tuples"),
)

process.EECTask = cms.Task(process.EEC2, process.genEEC2, process.EEC2Transfer)

#save EEC to nanoAOD
process.EEC2Table = cms.EDProducer("PatProjectedEECTableProducer",
    name = cms.string("EEC2"),
    jets = cms.InputTag(jets),
    EECs = cms.InputTag("EEC2"),
    nDR = cms.uint32(1),
)

#save gen EEC to nano
process.genEEC2Table = cms.EDProducer("GenProjectedEECTableProducer",
    name = cms.string("genEEC2"),
    jets = cms.InputTag(genJets),
    EECs = cms.InputTag("genEEC2"),
    nDR = cms.uint32(1),
)

#save transfer matrix to nano
process.transfer2Table = cms.EDProducer("EECTransferTableProducer",
    name = cms.string("transfer2"),
    transfer = cms.InputTag("EEC2Transfer"),
)

process.EECTableTask = cms.Task(process.EEC2Table, process.genEEC2Table, process.transfer2Table)

#muon selection
process.MyGoodMuons = cms.EDFilter("PATMuonSelector",
    src = cms.InputTag('slimmedMuons'),
    cut = cms.string("pt > 20 && abs(eta) < 2.7 && CutBasedIdTight && PFIsoTight"),
    filter = cms.bool(False),
)

process.ZTask = cms.Task(process.MyGoodMuons)

process.schedule.associate(process.ZTask, process.EMDTask, process.EECTask, process.EECTableTask)

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
#process.schedule = cms.Schedule(*[process.EEC_PATH, process.nanoAOD_step, process.endjob_step, process.NANOAODSIMoutput_step ], tasks=[process.customizedPFCandsTask, process.patAlgosToolsTask])
