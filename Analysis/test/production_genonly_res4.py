from SRothman.Analysis.common_cmsRun import *

from SRothman.Analysis.config.config import load_config
cfg = load_config('config_genonly')

# Input source
if input_fname is None:
    input_fname = "root://eoscms.cern.ch//store/cmst3/group/exovv/precision/dyjets_herwig/dyjets_herwig_1000.root"

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(input_fname),
    secondaryFileNames = cms.untracked.vstring(),
    #eventsToProcess = cms.untracked.VEventRange(cms.EventRange(1, 2339661, 1, 2339661))
)

# Path and EndPath definitions
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)
process.DroppedEventsSimOutput_step = cms.EndPath(process.DroppedEventsSimOutput)

from SRothman.Analysis.setupEventSelections_cff import setupEventSelections
process = setupEventSelections(process, 'genParticles', isMC=True,
                               genmuons=True,
                               config=cfg['EventSelection'])
# Schedule definition
process.schedule = cms.Schedule(process.selections_path,
                                process.NANOAODSIMoutput_step,
                                process.DroppedEventsSimOutput_step)

# customisation of the process.
from SRothman.Analysis.setupGenTables_cff import setupGenTables
process = setupGenTables(process)

from SRothman.Analysis.addParticlesTable_cff import addParticlesTable, addCollectionIndices
process = addParticlesTable(process, 
    "ZMuMu:daughters", 
    "ZMuMuMuons",
    singleton=False)
process = addCollectionIndices(process,
    "ZMuMu:daughters",
    "ZMuMuMuons",
    "genParticles"
)
process = addParticlesTable(process, 
    "ZMuMu:Z", 
    "ZMuMuZ", 
    singleton=True)

from SRothman.Analysis.setupAK8Jets_cff import setupAK8Jets
process = setupAK8Jets(process,
    isMC = True,
    skipJTB = True,
    genOnly = True,
    genParticles='genParticles',
    config=cfg)

from SRothman.CustomJets.setupSimonJets import setupSimonJets
process = setupSimonJets(process,
    jets = '',
    genjets = 'arbitratedGenJetsAK8',
    CHSjets = '',
    name = 'SimonJets',
    isMC = True,
    syst = "NOM",
    config=cfg,
    genOnly = True
)

#from SRothman.CustomJets.setupUniformGaussianJets import setupUniformGaussianJets
#process = setupUniformGaussianJets(process,
#    jets = '',
#    genjets = 'arbitratedGenJetsAK8',
#    CHSjets = '',
#    name = 'UniformGaussianJets',
#    isMC = True,
#    genOnly = True
#)

#from SRothman.CustomJets.setupFakeJets import setupFakeJets
#process = setupFakeJets(process,
#                        jets='',
#                        genjets='arbitratedGenJetsAK8',
#                        name='UUUFakeJets',
#                        isMC=True,
#                        genOnly=True,
#                        zmode='UNIFORM',
#                        thetamode="UNIFORM",
#                        phimode="UNIFORM")
#process = setupFakeJets(process,
#                        jets='',
#                        genjets='arbitratedGenJetsAK8',
#                        name='GUUFakeJets',
#                        isMC=True,
#                        genOnly=True,
#                        zmode='GLUON',
#                        thetamode="UNIFORM",
#                        phimode="UNIFORM")
#process = setupFakeJets(process,
#                        jets='',
#                        genjets='arbitratedGenJetsAK8',
#                        name='GLUFakeJets',
#                        isMC=True,
#                        genOnly=True,
#                        zmode='GLUON',
#                        thetamode="LNX",
#                        phimode="UNIFORM")
#process = setupFakeJets(process,
#                        jets='',
#                        genjets='arbitratedGenJetsAK8',
#                        name='GLCFakeJets',
#                        isMC=True,
#                        genOnly=True,
#                        zmode='GLUON',
#                        thetamode="LNX",
#                        phimode="COS2PHI")

from SRothman.EECs.setupEEC import setupEEC_data
process = setupEEC_data(process,
    name = 'EECs',
    recojets = 'GenSimonJets',
    verbose = 0,
    config = cfg['EECres4'],
    whichEEC='res4'
)

from SRothman.Analysis.addDeltaPsi import addDeltaPsi
process = addDeltaPsi(process, 'GenSimonJets')

#process = setupEECRes4_data(process,
#    name = 'UGEECs',
#    genMatch = '',
#    genjets = '',
#    recojets = 'GenUniformGaussianJets',
#    verbose = 0,
#    isMC = False
#)
#
#process = setupEECRes4_data(process,
#    name='UUUFakeEECs',
#    genMatch='',
#    genjets='',
#    recojets='GenUUUFakeJets',
#    verbose=0,
#    isMC=False
#)
#
#process = setupEECRes4_data(process,
#    name='GUUFakeEECs',
#    genMatch='',
#    genjets='',
#    recojets='GenGUUFakeJets',
#    verbose=0,
#    isMC=False
#)
#
#process = setupEECRes4_data(process,
#    name='GLUFakeEECs',
#    genMatch='',
#    genjets='',
#    recojets='GenGLUFakeJets',
#    verbose=0,
#    isMC=False
#)
#
#process = setupEECRes4_data(process,
#    name= 'GLCFakeEECs',
#    genMatch='',
#    genjets='',
#    recojets='GenGLCFakeJets',
#    verbose=0,
#    isMC=False
#)
#


process.HardShowerTreeInfoTable = cms.EDProducer("ShowerTreeInfoTableProducer",
    src = cms.InputTag("genParticles"),
    jets = cms.InputTag('arbitratedGenJetsAK8'),
    verbose = cms.int32(0),
    hardSide = cms.bool(True)
)
process.SoftShowerTreeInfoTable = cms.EDProducer("ShowerTreeInfoTableProducer",
    src = cms.InputTag("genParticles"),
    jets = cms.InputTag('arbitratedGenJetsAK8'),
    verbose = cms.int32(0),
    hardSide = cms.bool(False)
)
process.ShowerTreeInfoTask = cms.Task(
    process.HardShowerTreeInfoTable,
    process.SoftShowerTreeInfoTable
)
process.schedule.associate(process.ShowerTreeInfoTask)