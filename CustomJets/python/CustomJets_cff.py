import FWCore.ParameterSet.Config as cms
from SRothman.JetToolbox.jetToolbox_cff import jetToolbox
from SRothman.CustomJets.SimonJetProducer_cfi import *
from SRothman.CustomJets.SimonJetTableProducer_cfi import *

def addCustomJets(process, verbose=False):

    btags = [
        'pfDeepCSVJetTags:probb',
        'pfDeepCSVJetTags:probbb',
        'pfDeepCSVJetTags:probc',
        'pfDeepCSVJetTags:probudsg',
        'pfDeepFlavourJetTags:probb',
        'pfDeepFlavourJetTags:probbb',
        'pfDeepFlavourJetTags:problepb',
        'pfDeepFlavourJetTags:probc',
        'pfDeepFlavourJetTags:probuds',
        'pfDeepFlavourJetTags:probg'
    ]

    jetToolbox(process, "ak4", "customJets", "noOutput",
               PUMethod="Puppi", dataTier="nanoAOD",
               runOnMC=True, 
               JETCorrPayload="AK4PFPuppi",
               JETCorrLevels=["L1FastJet", "L2Relative","L3Absolute", "L2L3Residual"],
               Cut='',
               bTagDiscriminators = btags,
               GetSubjetMCFlavour=True,
               addPUJetID=True)
    #process.jetMC = cms.Sequence()
    #process.jetTables = cms.Sequence()
    #process.jetMCTable = process.patJetPartonsLegacy.clone()
    #process.nanoSequenceOnlyFullSim = cms.Sequence()
    process.patJetPartonsTask = cms.Task(process.patJetPartons)
    process.schedule.associate(process.patJetPartonsTask)

    process.SimonJets = PatSimonJetProducer.clone(
        src = "selectedPatJetsAK4PFPuppi",
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose,
    )
    process.GenSimonJets = GenSimonJetProducer.clone(
        src = "ak4GenJetsNoNu",
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose
    )
    process.SimonJetTable = SimonJetTableProducer.clone(
        src = "SimonJets",
        name = "SimonJets",
        verbose = verbose,
        isGen = False
    )
    process.GenSimonJetTable = SimonJetTableProducer.clone(
        src = "GenSimonJets",
        name = "GenSimonJets",
        verbose = verbose,
        isGen = True
    )
    process.JetsTask = cms.Task(process.SimonJets,
                                process.GenSimonJets)
    process.JetsTableTask = cms.Task(process.SimonJetTable,
                                     process.GenSimonJetTable)
    process.schedule.associate(process.JetsTask)
    process.schedule.associate(process.JetsTableTask)

    return process
