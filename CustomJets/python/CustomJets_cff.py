import FWCore.ParameterSet.Config as cms
from SRothman.JetToolbox.jetToolbox_cff import jetToolbox
from SRothman.CustomJets.SimonJetProducer_cfi import *
from SRothman.CustomJets.SimonJetTableProducer_cfi import *

def addCustomJets(process):

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
        src = "selectedPatJets"
    )
    process.GenSimonJets = GenSimonJetProducer.clone(
        src = "ak4GenJetsNoNu"
    )
    process.SimonJetTable = SimonJetTableProducer.clone(
        src = "SimonJets",
        name = "SimonJets"
    )
    process.GenSimonJetTable = SimonJetTableProducer.clone(
        src = "GenSimonJets",
        name = "GenSimonJets"
    )
    process.JetsTask = cms.Task(process.SimonJets,
                                process.GenSimonJets,
                                process.SimonJetTable,
                                process.GenSimonJetTable)
    process.schedule.associate(process.JetsTask)

    return process
