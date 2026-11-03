from SRothman.Analysis.common_cmsRun import *

from SRothman.Analysis.config.config import load_config
cfg = load_config('config_basic')

# Input source
if input_fname is None:
    input_fname = '/store/data/Run2018A/SingleMuon/MINIAOD/UL2018_MiniAODv2_GT36-v2/2820000/D60EE40A-46FC-3747-B58A-F00157E17671.root'

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(input_fname),
    secondaryFileNames = cms.untracked.vstring(),
    #eventsToProcess = cms.untracked.VEventRange(cms.EventRange(1, 2339661, 1, 2339661))
)

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '106X_dataRun2_v37', '')

# Shrink NANOAOD
from SRothman.Analysis.shrinkNano_cff import shrink_nanoAOD_data
process = shrink_nanoAOD_data(process)

# Path and EndPath definitions
process.nanoAOD_step = cms.Path(process.nanoSequence)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)

from SRothman.Analysis.setupEventSelections_cff import setupEventSelections, addSelectionTables
process, paths = setupEventSelections(
    process, 
    "linkedObjects:muons", 
    config=cfg['EventSelection'], 
    isMC=False, 
    genlevel=False,
    nameprefix='Reco',
    PAT=True
)
# Schedule definition
process.schedule = cms.Schedule(process.nanoAOD_step,
                                process.endjob_step,
                                process.NANOAODSIMoutput_step,
                                *paths)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)

# customisation of the process.

# Automatic addition of the customisation function from PhysicsTools.NanoAOD.nano_cff
from PhysicsTools.NanoAOD.nano_cff import nanoAOD_customizeData 

#call to customisation function nanoAOD_customizeData imported from PhysicsTools.NanoAOD.nano_cff
process = nanoAOD_customizeData(process)

#from SRothman.Analysis.setupRoccoR import setupRoccoR
#process = setupRoccoR(process, isMC=False)

process = addSelectionTables(process, 'Reco', 'linkedObjects:muons', genlevel=False)

from SRothman.Analysis.setupAK8Jets_cff import setupAK8Jets
process = setupAK8Jets(process,
   isMC = False,
   skipJTB = False,
   genOnly = False,
   config=cfg)

from SRothman.CustomJets.setupSimonJets import setupSimonJets

for syst in ['NOM', 'THRESH02', 'THRESH05', 'THRESH10', 'THRESH20']:
    suffix = syst.replace('_', '') # remove underscores for the suffix

    process = setupSimonJets(process,
        jets = 'selectedUpdatedJetsAK8',
        genjets = '',
        CHSjets = 'finalJets',
        name = 'ChargedSimonJets'+suffix,
        syst = syst,
        gensyst = 'NOM',
        isMC = False,
        genOnly = False,
        config=cfg
    )

    from SRothman.EECs.setupEEC import setupEEC_data
    process = setupEEC_data(process,
        name = 'ChargedEECs'+suffix,
        recojets = 'ChargedSimonJets'+suffix,
        whichEEC='res4',
        config = cfg['EECres4'],
        verbose = 0,
    )

# End of customisation functions

#process.SimpleMemoryCheck = cms.Service("SimpleMemoryCheck",
#    ignoreTotal = cms.untracked.int32(1)
#)

process.TriggerResultsHackTable = cms.EDProducer("TriggerResultsTableProducer",
    src = cms.InputTag("TriggerResults","","NANO"),
    name = cms.string("NANOTriggerResults"),
    verbose = cms.int32(0)
)
process.TriggerResultsHackTask = cms.Task(process.TriggerResultsHackTable)
process.schedule.associate(process.TriggerResultsHackTask)