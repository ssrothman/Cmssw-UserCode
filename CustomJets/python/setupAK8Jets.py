import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *
from SRothman.JetToolbox.jetToolbox_cff import jetToolbox

def setupAK8Jets(process):
    jetToolbox(process, 'ak8', 'dummy', 'noOutput',
               PUMethod='Puppi', dataTier='miniAOD',
               runOnMC=True, bTagDiscriminators = None,
               JETCorrPayload = 'AK8PFPuppi',
               JETCorrLevels = ['L1FastJet', 'L2Relative', 'L3Absolute'],
               GetJetMCFlavour = True,
               Cut = "pt > 20 && abs(eta) < 5.0")

    process.selectedGenJetsAK8 = cms.EDFilter("GenJetSelector",
        src = cms.InputTag("ak8GenJetsNoNu"),
        cut = cms.string("pt > 10.0 && abs(eta) < 5.0"),
        filter = cms.bool(False)
    )

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

    process.BigAK8JetTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
        src = cms.InputTag("selectedUpdatedJetsAK8"),
        cut = cms.string(""),
        name = cms.string('selectedJetsAK8'),
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
            jecFactor = Var("jecFactor('Uncorrected')", float, doc="jec factor", precision=-1),
            chHEF = Var("chargedHadronEnergyFraction()", float, doc='charged Hadron Energy Fraction', precision=-1),
            neHEF = Var("neutralHadronEnergyFraction()", float, doc='neutral Hadron Energy Fraction', precision=-1),
            chEmEF = Var("chargedEmEnergyFraction()", float, doc='charged EM Energy Fraction', precision=-1),
            neEmEF = Var("neutralEmEnergyFraction()", float, doc='neutral EM Energy Fraction', precision=-1),
            muEF = Var("muonEnergyFraction()", float, doc='muon Energy Fraction', precision=-1),
            hadronFlavour = Var("hadronFlavour", int, precision=-1),
            nBHadrons = Var("jetFlavourInfo().getbHadrons().size()", int, precision=-1),
            nCHadrons = Var("jetFlavourInfo().getcHadrons().size()", int, precision=-1),
        )
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
        )
    )
    
    process.selectedAK8GenJetFlavourTable = cms.EDProducer("GenJetFlavourTableProducer",
        cut = cms.string(""),
        deltaR = cms.double(0.1),
        jetFlavourInfos = cms.InputTag("selecteGenJetAK8FlavourAssociation"),
        name = cms.string("selectedGenJetsAK8"),
        src = cms.InputTag("selectedGenJetsAK8"),
    )

    process.selecteGenJetAK8FlavourAssociation = cms.EDProducer("JetFlavourClustering",
        bHadrons = cms.InputTag("patJetPartons","bHadrons"),
        cHadrons = cms.InputTag("patJetPartons","cHadrons"),
        ghostRescaling = cms.double(1e-18),
        hadronFlavourHasPriority = cms.bool(False),
        jetAlgorithm = cms.string('AntiKt'),
        jets = cms.InputTag("selectedGenJetsAK8"),
        leptons = cms.InputTag("patJetPartons","leptons"),
        partons = cms.InputTag("patJetPartons","physicsPartons"),
        rParam = cms.double(0.8)
    )
                                            
    process.ak8jetstask = cms.Task(
        process.selectedGenJetsAK8,
        process.jetIdLepVetoAK8,
        process.tightjetIdAK8,
        process.selectedUpdatedJetsAK8,
        process.BigAK8JetTable,
        process.BigAK8GenJetTable,
        process.selectedAK8GenJetFlavourTable,
        process.selecteGenJetAK8FlavourAssociation
    )

    process.schedule.associate(process.ak8jetstask)

    return process
