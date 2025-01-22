from SRothman.Analysis.common_cmsRun import *

if input_fname is None:
    input_fname = "root://eoscms.cern.ch//store/cmst3/group/exovv/precision/dyjets_herwig/dyjets_herwig_1000.root"
# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(input_fname),
    secondaryFileNames = cms.untracked.vstring(),
    #eventsToProcess = cms.untracked.VEventRange(cms.EventRange(1, 2339661, 1, 2339661))
)

# Path and EndPath definitions
process.NANOAODSIMoutput_step = cms.EndPath(process.NANOAODSIMoutput)
process.DroppedEventsSimOutput_step = cms.EndPath(process.DroppedEventsSimOutput)

from SRothman.Analysis.EventSelections_cff import setupEventSelections
process = setupEventSelections(process, isMC=True,
                               genmuons=True,
                               skipMET=True)
# Schedule definition
process.schedule = cms.Schedule(process.selections_path,
                                process.NANOAODSIMoutput_step,
                                    process.DroppedEventsSimOutput_step)

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
    genjets = 'arbitratedGenJetsAK8',
    CHSjets = '',
    chargedOnly = False,
    eventSelection = '',
    name = 'SimonJets',
    ak8 = True,
    isMC = True,
    genOnly = True
)

from SRothman.CustomJets.setupUniformGaussianJets import setupUniformGaussianJets
process = setupUniformGaussianJets(process,
    jets = '',
    genjets = 'arbitratedGenJetsAK8',
    CHSjets = '',
    chargedOnly = False,
    eventSelection = '',
    name = 'UniformGaussianJets',
    ak8 = True,
    isMC = True,
    genOnly = True
)

from SRothman.CustomJets.setupFakeJets import setupFakeJets
process = setupFakeJets(process,
                        jets='',
                        genjets='arbitratedGenJetsAK8',
                        name='UUUFakeJets',
                        isMC=True,
                        genOnly=True,
                        zmode='UNIFORM',
                        thetamode="UNIFORM",
                        phimode="UNIFORM")
process = setupFakeJets(process,
                        jets='',
                        genjets='arbitratedGenJetsAK8',
                        name='GUUFakeJets',
                        isMC=True,
                        genOnly=True,
                        zmode='GLUON',
                        thetamode="UNIFORM",
                        phimode="UNIFORM")
process = setupFakeJets(process,
                        jets='',
                        genjets='arbitratedGenJetsAK8',
                        name='GLUFakeJets',
                        isMC=True,
                        genOnly=True,
                        zmode='GLUON',
                        thetamode="LNX",
                        phimode="UNIFORM")
process = setupFakeJets(process,
                        jets='',
                        genjets='arbitratedGenJetsAK8',
                        name='GLCFakeJets',
                        isMC=True,
                        genOnly=True,
                        zmode='GLUON',
                        thetamode="LNX",
                        phimode="COS2PHI")

from SRothman.EECs.setupEECs import setupEECs
process = setupEECs(process,
    name = 'EECs',
    genMatch = '',
    genjets = '',
    recojets = 'GenSimonJets',
    verbose = 0,
    isMC = False
)

process = setupEECs(process,
    name = 'UGEECs',
    genMatch = '',
    genjets = '',
    recojets = 'GenUniformGaussianJets',
    verbose = 0,
    isMC = False
)

process = setupEECs(process,
    name='UUUFakeEECs',
    genMatch='',
    genjets='',
    recojets='GenUUUFakeJets',
    verbose=0,
    isMC=False
)

process = setupEECs(process,
    name='GUUFakeEECs',
    genMatch='',
    genjets='',
    recojets='GenGUUFakeJets',
    verbose=0,
    isMC=False
)

process = setupEECs(process,
    name='GLUFakeEECs',
    genMatch='',
    genjets='',
    recojets='GenGLUFakeJets',
    verbose=0,
    isMC=False
)

process = setupEECs(process,
    name= 'GLCFakeEECs',
    genMatch='',
    genjets='',
    recojets='GenGLCFakeJets',
    verbose=0,
    isMC=False
)

