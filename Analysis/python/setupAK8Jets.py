import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from SRothman.JetToolbox.jetToolbox_cff import jetToolbox

from SRothman.Analysis.config.config import config

def setupAK8GenJets(process, genParticles, partonMode):
    process.selectedGenJetsAK8 = cms.EDFilter("GenJetSelector",
        src = cms.InputTag("ak8GenJetsNoNu"),
        cut = cms.string("pt > %f &&"%config['GenJets']['GenJetPt'] +
                         "abs(eta) < %f"%config['GenJets']['GenJetEta']),
        filter = cms.bool(False)
    )

    process.BigAK8GenJetTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
        src = cms.InputTag("selectedGenJetsAK8"),
        cut = cms.string(""),
        name = cms.string('selectedGenJetsAK8'),
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
        jets = cms.InputTag("selectedGenJetsAK8"),
        leptons = cms.InputTag("genPartonsForFlavour","leptons"),
        partons = cms.InputTag("genPartonsForFlavour","physicsPartons"),
        rParam = cms.double(0.8)
    )

    process.selectedAK8GenJetFlavourTable = cms.EDProducer("GenJetFlavourTableProducer",
        cut = cms.string(""),
        deltaR = cms.double(0.1),
        jetFlavourInfos = cms.InputTag("selectedGenJetAK8FlavourAssociation"),
        name = cms.string("selectedGenJetsAK8"),
        src = cms.InputTag("selectedGenJetsAK8"),
    )

    process.MCak8jetstask = cms.Task(
        process.selectedGenJetsAK8,
        process.BigAK8GenJetTable,
        process.genPartonsForFlavour,
        process.selectedAK8GenJetFlavourTable,
        process.selectedGenJetAK8FlavourAssociation
    )
    process.schedule.associate(process.MCak8jetstask)

    return process

def setupAK8RecoJets(process):
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
             "abs(eta) < %f &&"%config['Jets']['JetEta'] + \
             "userInt('%s') &&"%config['Jets']['JetID'] + \
             "numberOfDaughters >= %d"%config['Jets']['JetNDaughters']
    jetcut = jetcut.encode('utf-8')

    process.actuallySelectedJetsAK8 = cms.EDFilter("PATJetSelector",
        src = cms.InputTag("selectedUpdatedJetsAK8"),
        cut = cms.string(jetcut),
        filter = cms.bool(False)
    )

    process.finalSelectedJetsAK8 = cms.EDFilter("PATJetOverlapCandidateVetoSelector",
        src = cms.InputTag("actuallySelectedJetsAK8"),
        vetoer = cms.InputTag("ZMuMu:daughters"),
        minDeltaR = cms.double(config['Jets']['JetMuonVetoDR']),
        filter = cms.bool(False),
        verbose = cms.int32(0)
    )

    process.BigAK8JetTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
        src = cms.InputTag("finalSelectedJetsAK8"),
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

    process.ak8jetstask = cms.Task(
        process.jetIdLepVetoAK8,
        process.tightjetIdAK8,
        process.selectedUpdatedJetsAK8,
        process.actuallySelectedJetsAK8,
        process.finalSelectedJetsAK8,
        process.BigAK8JetTable,
    )
    process.schedule.associate(process.ak8jetstask)

    return process

def setupAK8Jets(process, 
                 isMC,
                 skipJTB,
                 genOnly,
                 genParticles='prunedGenParticles',
                 partonMode='Auto'):
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
        setupAK8GenJets(process, genParticles, partonMode)

    if not genOnly:
        setupAK8RecoJets(process)

    return process
