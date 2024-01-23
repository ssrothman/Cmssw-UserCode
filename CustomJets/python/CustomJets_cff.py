import FWCore.ParameterSet.Config as cms
from  PhysicsTools.NanoAOD.common_cff import *

from SRothman.CustomJets.SimonJetProducer_cfi import *
from SRothman.CustomJets.ShadowJetProducer_cfi import *
from SRothman.CustomJets.FullEventJetProducer_cfi import *
from SRothman.CustomJets.SimonJetTableProducer_cfi import *

def setupCustomJets(process, verbose=False, table=False, 
                  EM0threshold = PatSimonJetProducer.EM0threshold,
                  HAD0threshold = PatSimonJetProducer.HAD0threshold,
                  HADCHthreshold = PatSimonJetProducer.HADCHthreshold,
                  ELEthreshold = PatSimonJetProducer.ELEthreshold,
                  MUthreshold = PatSimonJetProducer.MUthreshold,
                  onlyFromPV = PatSimonJetProducer.onlyFromPV,
                  onlyCharged = PatSimonJetProducer.onlyCharged,

                  maxNumPart= PatSimonJetProducer.maxNumPart,
                  minNumPart= PatSimonJetProducer.minNumPart,
                  minPt= PatSimonJetProducer.minPt,
                  maxEta= PatSimonJetProducer.maxEta,
                  maxMuFrac= PatSimonJetProducer.maxMuFrac,
                  maxChEmFrac= PatSimonJetProducer.maxChEmFrac,

                  minPartPt_GEN=GenSimonJetProducer.HADCHthreshold, 
                  maxEta_GEN=GenSimonJetProducer.maxEta,
                  minPt_GEN=GenSimonJetProducer.minPt,

                  inputJets = 'patJetsReapplyJECPuppi',
                  jets = 'updatedJetsPuppi',
                  CHSjets = 'finalJets',
                  parts = 'packedPFCandidates',
                  genJets = 'slimmedGenJets',
                  genParts = 'packedGenParticles'):

    process.jetIdLepVetoPuppi = cms.EDProducer("PatJetIDValueMapProducer",
        filterParams = cms.PSet(
            quality = cms.string("TIGHTLEPVETO"),
            version = cms.string("RUN2ULPUPPI"),
        ),
        src = cms.InputTag(inputJets)
    )

    from RecoJets.JetProducers.PileupJetID_cfi import _chsalgos_106X_UL18
    process.pileupJetIdPuppi = process.pileupJetId.clone( 
        jets=cms.InputTag(inputJets),
        inputIsCorrected=True,
        applyJec=False,
        vertexes=cms.InputTag("offlineSlimmedPrimaryVertices"),
        algos = cms.VPSet(_chsalgos_106X_UL18),
    )

    process.updatedJetsPuppi = cms.EDProducer("PATJetUserDataEmbedder",
        src = cms.InputTag(inputJets),
        userInts = cms.PSet(
            puID = cms.InputTag("pileupJetIdPuppi:fullId"),
            jetIdLepVeto = cms.InputTag("jetIdLepVetoPuppi"),
        ) 
    )

    process.selectedGenJets = cms.EDFilter("GenJetSelector",
        src = cms.InputTag(genJets),
        cut = cms.string("pt > 10.0 && abs(eta) < 5.0"),
        filter = cms.bool(False)
    )

    process.extraJetTask = cms.Task(
        process.pileupJetIdPuppi,
        process.jetIdLepVetoPuppi,
        process.updatedJetsPuppi,
        process.selectedGenJets
    )
    process.schedule.associate(process.extraJetTask)

    process.SimonJets = PatSimonJetProducer.clone(
        jetSrc = jets,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        CHSsrc = CHSjets,
        addCHSindex = True,
        verbose = verbose,
        EM0threshold = EM0threshold,
        HAD0threshold = HAD0threshold,
        HADCHthreshold = HADCHthreshold,
        ELEthreshold = ELEthreshold,
        MUthreshold = MUthreshold,
        maxNumPart = maxNumPart,
        minNumPart = minNumPart,
        minPt = minPt,
        maxEta = maxEta,
        maxMuFrac = maxMuFrac,
        maxChEmFrac = maxChEmFrac,
        onlyFromPV = onlyFromPV,
        onlyCharged = onlyCharged,
    )
    process.GenSimonJets = GenSimonJetProducer.clone(
        jetSrc = genJets,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        addCHSindex = False,
        verbose = verbose,
        EM0threshold = minPartPt_GEN,
        HAD0threshold = minPartPt_GEN,
        HADCHthreshold = minPartPt_GEN,
        ELEthreshold = minPartPt_GEN,
        MUthreshold = minPartPt_GEN,
        minPt = minPt_GEN, 
        maxEta = maxEta_GEN,
        maxNumPart = maxNumPart,
        onlyFromPV = False,
        onlyCharged = onlyCharged,
    )

    '''process.GenFullEventJets = FullEventJetProducer.clone(
        partSrc = genParts,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose,
        minPartPt = minPartPt_GEN,
    )'''

    '''process.GenShadowJets = CandidateShadowJetProducer.clone(
        jetSrc = "SimonJets",
        partSrc = genParts,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose,
        minPartPt = minPartPt_GEN,
    )'''

    '''process.FullEventJets = FullEventJetProducer.clone(
        partSrc = parts,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose,
        minPartPt = -1,
    )'''

    '''process.ShadowJets = RecoShadowJetProducer.clone(
        jetSrc = "GenSimonJets",
        partSrc = parts,
        eventSelection = "ZMuMu",
        doEventSelection = True,
        verbose = verbose,
        minPartPt = -1,
    )'''

    process.JetsTask = cms.Task(process.SimonJets,
                                process.GenSimonJets,
                                #process.FullEventJets,
                                #process.GenFullEventJets,
                                #process.ShadowJets,
                                #process.GenShadowJets
                        )
    process.schedule.associate(process.JetsTask)

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

        '''process.FullEventJetTable = SimonJetTableProducer.clone(
            src = "FullEventJets",
            name = "FullEventJets",
            verbose = verbose,
            isGen = False
        )'''
        '''process.GenFullEventJetTable = SimonJetTableProducer.clone(
            src = "GenFullEventJets",
            name = "GenFullEventJets",
            verbose = verbose,
            isGen = True
        )'''
        '''process.ShadowJetTable = SimonJetTableProducer.clone(
            src = "ShadowJets",
            name = "ShadowJets",
            verbose = verbose,
            isGen = False
        )'''
        '''process.GenShadowJetTable = SimonJetTableProducer.clone(
            src = "GenShadowJets",
            name = "GenShadowJets",
            verbose = verbose,
            isGen = True
        )'''

        process.FullJetTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
            src = cms.InputTag(jets),
            cut = cms.string(''),
            name = cms.string(jets),
            singleton = cms.bool(False),
            extension = cms.bool(False),
            externalVariables = cms.PSet(), #add btag here?
            variables = cms.PSet(
                pt = Var("pt()", float, doc="pT of AK4 jet",precision=-1),
                eta = Var("eta()", float, doc="eta of AK4 jet",precision=-1),
                phi = Var("phi()", float, doc="phi of AK4 jet",precision=-1),
                area = Var("jetArea()", float, doc="jet catchment area, for JECs",precision=-1),
                jetIdLepVeto = Var("userInt('jetIdLepVeto')",int,doc="Jet ID flags bit1 is loose (always false in 2017 since it does not exist), bit2 is tight, bit3 is tightLepVeto"),
                puId = Var("userInt('puID')", int,doc="Pileup ID flags with 106X (2018) training"),
                nConstituents = Var("numberOfDaughters()","uint8",doc="Number of particles in the jet"),
                jecFactor = Var("jecFactor('Uncorrected')",float,doc="Factor to get back to raw pT",precision=-1),
                chHEF = Var("chargedHadronEnergyFraction()", float, doc="charged Hadron Energy Fraction", precision=-1),
                neHEF = Var("neutralHadronEnergyFraction()", float, doc="neutral Hadron Energy Fraction", precision=-1),
                chEmEF = Var("chargedEmEnergyFraction()", float, doc="charged Electromagnetic Energy Fraction", precision=-1),
                neEmEF = Var("neutralEmEnergyFraction()", float, doc="neutral Electromagnetic Energy Fraction", precision=-1),
                muEF = Var("muonEnergyFraction()", float, doc="muon Energy Fraction", precision=-1),
                #chFPV0EF = Var("userFloat('chFPV0EF')", float, doc="charged fromPV==0 Energy Fraction (energy excluded from CHS jets). Previously called betastar.", precision= 6),
            )
        )

        process.JetsTableTask = cms.Task(process.SimonJetTable,
                                         process.GenSimonJetTable,
                                         #process.FullEventJetTable,
                                         #process.GenFullEventJetTable,
                                         #process.ShadowJetTable,
                                         #process.GenShadowJetTable,
                                         process.FullJetTable)
        process.schedule.associate(process.JetsTableTask)

    return process
