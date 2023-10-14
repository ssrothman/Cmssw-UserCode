import FWCore.ParameterSet.Config as cms
from SRothman.JetToolbox.jetToolbox_cff import jetToolbox
from SRothman.CustomJets.SimonJetProducer_cfi import *
from SRothman.CustomJets.ShadowJetProducer_cfi import *
from SRothman.CustomJets.FullEventJetProducer_cfi import *
from SRothman.CustomJets.SimonJetTableProducer_cfi import *

def addCustomJets(process, verbose=False, table=False):

    #jets = 'selectedPatJetsAK4PFPuppi'
    #parts = 'puppi'
    #genJets = 'ak4GenJetsNoNu'
    #genParts = 'packedGenParticlesForJetsNoNu'
    #datatier = 'nanoAOD'
    
    jets = 'selectedPatJetsAK4PFPuppi'
    parts = 'puppi'
    genJets = 'ak4GenJetsNoNu'
    genParts = 'genParticlesForJetsNoNuTMP'
    datatier = 'AOD'

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
               PUMethod="Puppi", dataTier=datatier,
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
        src = jets,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose,
        minPartPt = 0.0,
    )
    process.GenSimonJets = GenSimonJetProducer.clone(
        minPartPt = 0.0,
        src = genJets,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose
    )

    process.GenFullEventJets = CandidateFullEventJetProducer.clone(
        minPartPt = 0.0,
        partSrc = genParts,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose
    )

    process.GenShadowJets = CandidateShadowJetProducer.clone(
        minPartPt = 0.0,
        jetSrc = "SimonJets",
        partSrc = genParts,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose
    )

    process.FullEventJets = RecoFullEventJetProducer.clone(
        minPartPt = 0.0,
        partSrc = parts,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose
    )

    process.ShadowJets = RecoShadowJetProducer.clone(
        minPartPt = 0.0,
        jetSrc = "GenSimonJets",
        partSrc = parts,
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
