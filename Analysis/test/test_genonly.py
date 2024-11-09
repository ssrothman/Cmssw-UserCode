from SRothman.Analysis.common_cmsRun import *

# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring("root://eoscms.cern.ch//store/cmst3/group/exovv/precision/dyjets_herwig/dyjets_herwig_1000.root"),
    secondaryFileNames = cms.untracked.vstring(),
    #eventsToProcess = cms.untracked.VEventRange(cms.EventRange(1, 2339661, 1, 2339661))
)

# Path and EndPath definitions
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)
process.DroppedEventsOutput_step = cms.EndPath(process.DroppedEventsOutput)

from SRothman.Analysis.EventSelections_cff import setupEventSelections
process = setupEventSelections(process, isMC=True,
                               genmuons=True,
                               skipMET=True)
# Schedule definition
process.schedule = cms.Schedule(process.selections_path,
                                process.NANOAODSIMoutput_step,
                                process.DroppedEventsOutput_step)

# customisation of the process.
from SRothman.Analysis.setupGenTables import setupGenTables
process = setupGenTables(process)

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
    skipJTB = True,
    genOnly = True,
    genParticles='genParticles',
    applyExtraGenSelections=True)

from SRothman.CustomJets.setupSimonJets import setupSimonJets
process = setupSimonJets(process,
    jets = '',
    genjets = 'selectedGenJetsAK8',
    CHSjets = '',
    chargedOnly = True,
    eventSelection = '',
    name = 'ChargedSimonJets',
    ak8 = True,
    isMC = True,
    genOnly = True
)

from SRothman.EECs.setupEECs import setupEECs
process = setupEECs(process,
    name = 'EECs',
    genMatch = '',
    genjets = '',
    recojets = 'GenChargedSimonJets',
    verbose = 0,
    isMC = False
)

# End of customisation functions
