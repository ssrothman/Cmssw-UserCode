import FWCore.ParameterSet.Config as cms
from SRothman.JetToolbox.jetToolbox_cff import jetToolbox
from SRothman.CustomJets.SimonJetProducer_cfi import *
from SRothman.CustomJets.ShadowJetProducer_cfi import *
from SRothman.CustomJets.FullEventJetProducer_cfi import *
from SRothman.CustomJets.SimonJetTableProducer_cfi import *

def addCustomJets(process, verbose=False, table=False):

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
    btags = None

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
        minPartPt = 1.0,
    )
    process.GenSimonJets = GenSimonJetProducer.clone(
        minPartPt = 0.0,
        src = "ak4GenJetsNoNu",
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose
    )

    process.GenFullEventJets = CandidateFullEventJetProducer.clone(
        minPartPt = 0.0,
        partSrc = "packedGenParticlesForJetsNoNu",
        #jetSrc = "SimonJets",
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose
    )

    process.GenShadowJets = CandidateShadowJetProducer.clone(
        minPartPt = 0.0,
        jetSrc = "SimonJets",
        partSrc = "packedGenParticlesForJetsNoNu",
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose
    )

    process.FullEventJets = RecoFullEventJetProducer.clone(
        minPartPt = 1.0,
        partSrc = "puppi",
        #jetSrc = "GenSimonJets",
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose
    )

    process.ShadowJets = RecoShadowJetProducer.clone(
        minPartPt = 1.0,
        jetSrc = "GenSimonJets",
        partSrc = "puppi",
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose
    )

    if table:
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
        process.FullEventJetTable = SimonJetTableProducer.clone(
            src = "FullEventJets",
            name = "FullEventJets",
            verbose = verbose,
            isGen = False
        )
        process.GenFullEventJetTable = SimonJetTableProducer.clone(
            src = "GenFullEventJets",
            name = "GenFullEventJets",
            verbose = verbose,
            isGen = True
        )
        process.ShadowJetTable = SimonJetTableProducer.clone(
            src = "ShadowJets",
            name = "ShadowJets",
            verbose = verbose,
            isGen = False
        )
        process.GenShadowJetTable = SimonJetTableProducer.clone(
            src = "GenShadowJets",
            name = "GenShadowJets",
            verbose = verbose,
            isGen = True
        )

        process.JetsTableTask = cms.Task(process.SimonJetTable,
                                         process.GenSimonJetTable,
                                         process.FullEventJetTable,
                                         process.GenFullEventJetTable,
                                         process.ShadowJetTable,
                                         process.GenShadowJetTable)
        process.schedule.associate(process.JetsTableTask)

    process.JetsTask = cms.Task(process.SimonJets,
                                process.GenSimonJets,
                                process.FullEventJets,
                                process.GenFullEventJets,
                                process.ShadowJets,
                                process.GenShadowJets)
    process.schedule.associate(process.JetsTask)

    return process
