from SRothman.Analysis.common_cmsRun import *

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_upgrade2018_realistic_v16_L1v1', '')

# Shrink NANOAOD
from SRothman.Analysis.shrinkNano import shrink_nanoAOD_MC
process = shrink_nanoAOD_MC(process)

# Path and EndPath definitions
process.nanoAOD_step = cms.Path(process.nanoSequenceMC)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)
process.DroppedEventsOutput_step = cms.EndPath(process.DroppedEventsOutput)

from SRothman.Analysis.EventSelections_cff import setupEventSelections
process = setupEventSelections(process, isMC=True)
# Schedule definition
process.schedule = cms.Schedule(process.selections_path,
                                process.nanoAOD_step,
                                process.endjob_step,
                                process.NANOAODSIMoutput_step)
                                #process.DroppedEventsOutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

# customisation of the process.

# Automatic addition of the customisation function from PhysicsTools.NanoAOD.nano_cff
from PhysicsTools.NanoAOD.nano_cff import nanoAOD_customizeMC 

#call to customisation function nanoAOD_customizeMC imported from PhysicsTools.NanoAOD.nano_cff
process = nanoAOD_customizeMC(process)

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
   isMC = True,
   skipJTB = False,
   genOnly = False)

from SRothman.CustomJets.setupSimonJets import setupSimonJets
process = setupSimonJets(process,
    jets = 'finalSelectedJetsAK8',
    genjets = 'selectedGenJetsAK8', 
    CHSjets = 'finalJets',
    chargedOnly = True,
    eventSelection = '',
    name = 'ChargedSimonJets',
    ak8 = True,
    isMC = True,
    genOnly = False
)

from SRothman.Matching.setupMatching import setupMatching
process = setupMatching(process,
    verbose = 0,
    ak8 = True,
    name = 'GenMatch',
    reco = 'ChargedSimonJets',
    gen = 'GenChargedSimonJets',
    naive = False
)

from SRothman.EECs.setupEECs import setupEECs
process = setupEECs(process,
    name = 'EECs',
    genMatch = 'GenMatch',
    genjets = 'GenChargedSimonJets',
    recojets = 'ChargedSimonJets',
    verbose = 0,
    isMC = True
)

# End of customisation functions
