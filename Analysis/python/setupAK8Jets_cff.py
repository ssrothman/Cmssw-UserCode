import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from SRothman.JetToolbox.jetToolbox_cff import jetToolbox

from SRothman.Analysis.config.config import config

def setupAK8GenJets(process, genParticles, partonMode,
                    applyExtraGenSelections):

    print("")
    print(" --------- SETTING UP GEN JETS --------- ")
    print("")

    cutstring = "pt > %g &&"%config['GenJets']['GenJetPt'] + \
                " abs(eta) < %g"%config['GenJets']['GenJetEta']

    if applyExtraGenSelections: #fakes the tight jet ID + lepton veto
        cutstring += " && muonEnergy/(pt*cosh(eta)) < %g"%config['GenJets']['muEF']
        cutstring += " && chargedEmEnergy/(pt*cosh(eta)) < %g"%config['GenJets']['chEmEF']
        cutstring += " && neutralEmEnergy/(pt*cosh(eta)) < %g"%config['GenJets']['nEmEF']
        cutstring += " && neutralHadronEnergy/(pt*cosh(eta)) < %g"%config['GenJets']['nHadEF']
        cutstring += ' && numberOfDaughters > %d'%config['GenJets']['nConstituents']

    print("Cutstring: %s"%cutstring)

    process.cutGenJetsAK8 = cms.EDFilter("GenJetSelector",
        src = cms.InputTag("ak8GenJetsNoNu"),
        cut = cms.string(cutstring),
        filter = cms.bool(False)
    )

    if not applyExtraGenSelections:
        process.selectedGenJetsAK8 = cms.EDFilter("GenJetSelector",
            src = cms.InputTag("cutGenJetsAK8"),
            cut = cms.string(""),
            filter = cms.bool(False)
        )
        print("NO overlap veto")
    else:
        process.selectedGenJetsAK8 = cms.EDFilter("GENJetOverlapCandidateVetoSelector",
            src = cms.InputTag("cutGenJetsAK8"),
            vetoer = cms.InputTag("ZMuMu:daughters"),
            minDeltaR = cms.double(config['Jets']['JetMuonVetoDR']),
            filter = cms.bool(False),
            verbose = cms.int32(0),
            makeValueMap = cms.bool(False)
        )
        print("YES overlap veto")

    if applyExtraGenSelections:
        arbitration = config['Jets']['Arbitration']
        if arbitration.startswith("Leading"):
            process.arbitratedGenJetsAK8 = cms.EDFilter("LeadingGenJetSelector",
                src = cms.InputTag("selectedGenJetsAK8"),
                maxNumber = cms.uint32(int(arbitration[7:])),
                filter = cms.bool(False)
            )
            print("Applying %s arbitration"%arbitration)
        elif arbitration == 'None':
            process.arbitratedGenJetsAK8 = cms.EDFilter("GenJetSelector",
                src = cms.InputTag("selectedGenJetsAK8"),
                cut = cms.string(""),
                filter = cms.bool(False)
            )
            print("No jet arbitration")
        else:
            raise ValueError("Invalid jet arbitration %s"%arbitration)
    else:
        process.arbitratedGenJetsAK8 = cms.EDFilter("GenJetSelector",
            src = cms.InputTag("selectedGenJetsAK8"),
            cut = cms.string(""),
            filter = cms.bool(False)
        )
        print("No jet arbitration")

    process.BigAK8GenJetTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
        src = cms.InputTag("arbitratedGenJetsAK8"),
        cut = cms.string(""),
        name = cms.string('arbitratedGenJetsAK8'),
        singleton = cms.bool(False),
        extension = cms.bool(False),
        externalVariables = cms.PSet(),
        variables = cms.PSet(
            pt = Var("pt", float, doc="transverse momentum", precision=-1),
            eta = Var("eta", float, doc="pseudorapidity", precision=-1),
            phi = Var("phi", float, doc="azimuthal angle", precision=-1),
            mass = Var("mass", float, doc="mass", precision=-1),
            area = Var("jetArea", float, doc="jet area", precision=-1),
            nConstituents = Var("numberOfDaughters", int, doc="number of jet constituents", precision=-1),
            chargedHadronEnergy = Var("chargedHadronEnergy", float, doc="charged Hadron Energy", precision=-1),
            neutralHadronEnergy = Var("neutralHadronEnergy", float, doc="neutral Hadron Energy", precision=-1),
            chargedEmEnergy = Var("chargedEmEnergy", float, doc="charged EM Energy", precision=-1),
            neutralEmEnergy = Var("neutralEmEnergy", float, doc="neutral EM Energy", precision=-1),
            muonEnergy = Var("muonEnergy", float, doc="muon Energy", precision=-1),
            chargedHadronMultiplicity = Var("chargedHadronMultiplicity", int, doc="charged Hadron Multiplicity", precision=-1),
            neutralHadronMultiplicity = Var("neutralHadronMultiplicity", int, doc="neutral Hadron Multiplicity", precision=-1),
            chargedEmMultiplicity = Var("chargedEmMultiplicity", int, doc="charged EM Multiplicity", precision=-1),
            neutralEmMultiplicity = Var("neutralEmMultiplicity", int, doc="neutral EM Multiplicity", precision=-1),
            muonMultiplicity = Var("muonMultiplicity", int, doc="muon Multiplicity", precision=-1),
        )
    )

    process.genPartonsForFlavour = cms.EDProducer("HadronAndPartonSelector",
        fullChainPhysPartons = cms.bool(False),
        particles = cms.InputTag(genParticles),
        partonMode = cms.string(partonMode),
        src = cms.InputTag("generator"),
    )

    process.selectedGenJetAK8FlavourAssociation = cms.EDProducer("JetFlavourClustering",
        bHadrons = cms.InputTag("genPartonsForFlavour","bHadrons"),
        cHadrons = cms.InputTag("genPartonsForFlavour","cHadrons"),
        ghostRescaling = cms.double(1e-18),
        hadronFlavourHasPriority = cms.bool(True),
        jetAlgorithm = cms.string('AntiKt'),
        jets = cms.InputTag("arbitratedGenJetsAK8"),
        leptons = cms.InputTag("genPartonsForFlavour","leptons"),
        partons = cms.InputTag("genPartonsForFlavour","physicsPartons"),
        rParam = cms.double(0.8)
    )

    process.selectedAK8GenJetFlavourTable = cms.EDProducer("GenJetFlavourTableProducer",
        cut = cms.string(""),
        deltaR = cms.double(0.1),
        jetFlavourInfos = cms.InputTag("selectedGenJetAK8FlavourAssociation"),
        name = cms.string("arbitratedGenJetsAK8"),
        src = cms.InputTag("arbitratedGenJetsAK8"),
    )

    process.MCak8jetstask = cms.Task(
        process.cutGenJetsAK8,
        process.arbitratedGenJetsAK8,
        process.selectedGenJetsAK8,
        process.BigAK8GenJetTable,
        process.genPartonsForFlavour,
        process.selectedAK8GenJetFlavourTable,
        process.selectedGenJetAK8FlavourAssociation
    )
    process.schedule.associate(process.MCak8jetstask)

    print("")
    print(" --------- FINISHED SETTING UP GEN JETS --------- ")
    print("")

    return process

def setupAK8RecoJets(process):
    print("")
    print(" --------- SETTING UP RECO JETS --------- ")
    print("")

    process.jetIdLepVetoAK8 = cms.EDProducer("PatJetIDValueMapProducer",
        filterParams = cms.PSet(
            quality = cms.string("TIGHTLEPVETO"),
            version = cms.string("RUN2ULPUPPI"),
        ),
        src = cms.InputTag("selectedPatJetsAK8PFPuppi"),
    )

    process.tightjetIdAK8 = cms.EDProducer("PatJetIDValueMapProducer",
        filterParams = cms.PSet(
            quality = cms.string("TIGHT"),
            version = cms.string("RUN2ULPUPPI"),
        ),
        src = cms.InputTag("selectedPatJetsAK8PFPuppi"),
    )

    process.selectedUpdatedJetsAK8 = cms.EDProducer("PATJetUserDataEmbedder",
        src = cms.InputTag("selectedPatJetsAK8PFPuppi"),
        userInts = cms.PSet(
            jetIdLepVeto = cms.InputTag("jetIdLepVetoAK8"),
            jetIdTight = cms.InputTag("tightjetIdAK8"),
        )
    )

    jetcut = "pt > %f &&"%config['Jets']['JetPt'] + \
             " abs(eta) < %f &&"%config['Jets']['JetEta'] + \
             " userInt('%s') &&"%config['Jets']['JetID'] + \
             " numberOfDaughters >= %d"%config['Jets']['JetNDaughters']
    print("custring: %s"%jetcut)

    process.preselectJetsAK8 = cms.EDProducer("PATJetSelectionFlagProducer",
        src = cms.InputTag("selectedUpdatedJetsAK8"),
        cut = cms.string(jetcut),
        verbose = cms.int32(0)
    )      

    process.overlapVetoJetsAK8 = cms.EDFilter("PATJetOverlapCandidateVetoSelector",
        src = cms.InputTag("selectedUpdatedJetsAK8"),
        vetoer = cms.InputTag("ZMuMu:daughters"),
        minDeltaR = cms.double(config['Jets']['JetMuonVetoDR']),
        filter = cms.bool(False),
        makeValueMap = cms.bool(True),
        verbose = cms.int32(0)
    )
    print("YES overlap veto")

    process.BigAK8JetTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
        src = cms.InputTag("selectedUpdatedJetsAK8"),
        cut = cms.string(""),
        name = cms.string('finalSelectedJetsAK8'),
        singleton = cms.bool(False),
        extension = cms.bool(False),
        externalVariables = cms.PSet(),
        variables = cms.PSet(
            pt = Var("pt", float, doc="transverse momentum", precision=-1),
            eta = Var("eta", float, doc="pseudorapidity", precision=-1),
            phi = Var("phi", float, doc="azimuthal angle", precision=-1),
            mass = Var("mass", float, doc="mass", precision=-1),
            area = Var("jetArea", float, doc="jet area", precision=-1),
            jetIdLepVeto = Var("userInt('jetIdLepVeto')", int),
            jetIdTight = Var("userInt('jetIdTight')", int),
            nConstituents = Var("numberOfDaughters", int, doc="number of jet constituents", precision=-1),
            jecFactor = Var("jecFactor('Uncorrected')", float, doc="jec factor", precision=-1),
            chHEF = Var("chargedHadronEnergyFraction()", float, doc='charged Hadron Energy Fraction', precision=-1),
            neHEF = Var("neutralHadronEnergyFraction()", float, doc='neutral Hadron Energy Fraction', precision=-1),
            chEmEF = Var("chargedEmEnergyFraction()", float, doc='charged EM Energy Fraction', precision=-1),
            neEmEF = Var("neutralEmEnergyFraction()", float, doc='neutral EM Energy Fraction', precision=-1),
            muEF = Var("muonEnergyFraction()", float, doc='muon Energy Fraction', precision=-1),
            hadronFlavour = Var("hadronFlavour", int, precision=-1),
            partonFlavour = Var("partonFlavour", int, precision=-1),
            nBHadrons = Var("jetFlavourInfo().getbHadrons().size()", int, precision=-1),
            nCHadrons = Var("jetFlavourInfo().getcHadrons().size()", int, precision=-1),
        )
    )
    process.BigAK8JetTable.externalVariables.overlapVeto = cms.PSet(
        compression = cms.string('none'),
        doc = cms.string("Jet/Z->mumu overlap veto"),
        mcOnly = cms.bool(False),
        precision = cms.int32(-1),
        src = cms.InputTag("overlapVetoJetsAK8"),
        type = cms.string('bool')
    )
    process.BigAK8JetTable.externalVariables.preselection = cms.PSet(
        compression = cms.string('none'),
        doc = cms.string("Jet preselection"),
        mcOnly = cms.bool(False),
        precision = cms.int32(-1),
        src = cms.InputTag("preselectJetsAK8"),
        type = cms.string('bool')
    )

    process.ak8jetstask = cms.Task(
        process.jetIdLepVetoAK8,
        process.tightjetIdAK8,
        process.selectedUpdatedJetsAK8,
        process.preselectJetsAK8,
        process.overlapVetoJetsAK8,
        process.BigAK8JetTable,
    )
    process.schedule.associate(process.ak8jetstask)

    print("")
    print(" --------- FINISHED SETTING UP RECO JETS --------- ")
    print("")

    return process

def setupAK8Jets(process, 
                 isMC,
                 skipJTB,
                 genOnly,
                 genParticles='prunedGenParticles',
                 partonMode='Auto',
                 applyExtraGenSelections=False):
    if not skipJTB:
        jetToolbox(process, 'ak8', 'dummy', 'noOutput',
                   PUMethod='Puppi', dataTier='miniAOD',
                   bTagDiscriminators = None,
                   JETCorrPayload = 'AK8PFPuppi',
                   JETCorrLevels = ['L1FastJet', 'L2Relative', 'L3Absolute'],
                   GetJetMCFlavour = True,
                   Cut = "pt > %f && abs(eta) < %f"%(config['GenJets']['GenJetPt'], config['GenJets']['GenJetEta']), 
                   runOnMC=isMC)
    
    if isMC:
        setupAK8GenJets(process, genParticles, partonMode,
                        applyExtraGenSelections)

    if not genOnly:
        setupAK8RecoJets(process)

    return process
