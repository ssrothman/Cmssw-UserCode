from SRothman.Analysis.common_cmsRun import *

# Input source
if input_fname is None:
    input_fname = '/store/data/Run2018A/SingleMuon/MINIAOD/UL2018_MiniAODv2_GT36-v1/2820000/000EE25A-A8E8-1444-8A0B-0DBEBE5634FB.root'

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(input_fname),
    secondaryFileNames = cms.untracked.vstring(),
    #eventsToProcess = cms.untracked.VEventRange(cms.EventRange(1, 2339661, 1, 2339661))
)

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_dataRun2_v37', '')

# Shrink NANOAOD
from SRothman.Analysis.shrinkNano import shrink_nanoAOD_data
process = shrink_nanoAOD_data(process)

# Path and EndPath definitions
process.nanoAOD_step = cms.Path(process.nanoSequence)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)

from SRothman.Analysis.EventSelections_cff import setupEventSelections
process = setupEventSelections(process, isMC=False)
# Schedule definition
process.schedule = cms.Schedule(process.selections_path,
                                process.nanoAOD_step,
                                process.endjob_step,
                                process.NANOAODSIMoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

# customisation of the process.

# Automatic addition of the customisation function from PhysicsTools.NanoAOD.nano_cff
from PhysicsTools.NanoAOD.nano_cff import nanoAOD_customizeData 

#call to customisation function nanoAOD_customizeData imported from PhysicsTools.NanoAOD.nano_cff
process = nanoAOD_customizeData(process)

from SRothman.Analysis.addParticlesTable import addParticlesTable
process = addParticlesTable(process, 
    "ZMuMu:daughters", 
    "ZMuMuMuons",
    singleton=False)
process = addParticlesTable(process, 
    "ZMuMu:Z", 
    "ZMuMuZ", 
    singleton=True)

from SRothman.Analysis.setupAK8Jets import setupAK8Jets
process = setupAK8Jets(process,
   isMC = False,
   skipJTB = False,
   genOnly = False)

from SRothman.CustomJets.setupSimonJets import setupSimonJets
process = setupSimonJets(process,
    jets = 'finalSelectedJetsAK8',
    genjets = '',
    CHSjets = 'finalJets',
    chargedOnly = True,
    eventSelection = '',
    name = 'ChargedSimonJets',
    ak8 = True,
    isMC = False,
    genOnly = False
)

process = setupSimonJets(process,
    jets = 'finalSelectedJetsAK8',
    genjets = '',
    CHSjets = 'finalJets',
    chargedOnly = False,
    eventSelection = '',
    name = 'SimonJets',
    ak8 = True,
    isMC = False,
    genOnly = False
)

from SRothman.EECs.setupEECs import setupEECs
process = setupEECs(process,
    name = 'ChargedEECs',
    genMatch = '',
    genjets = '',
    recojets = 'ChargedSimonJets',
    verbose = 0,
    isMC = False
)

process = setupEECs(process,
    name = 'EECs',
    genMatch = '',
    genjets = '',
    recojets = 'SimonJets',
    verbose = 0,
    isMC = False
)

# End of customisation functions
