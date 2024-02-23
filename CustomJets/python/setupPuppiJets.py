import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *

def setupPuppiJets(process, 
                   inputJets = 'patJetsReapplyJECPuppi',
                   inputGenJets = 'slimmedGenJets',
                   isMC=True):

    if isMC:
        process.selectedGenJets = cms.EDFilter("GenJetSelector",
            src = cms.InputTag(inputGenJets),
            cut = cms.string("pt > 10.0 && abs(eta) < 5.0"),
            filter = cms.bool(False)
        )

    process.jetIdLepVetoPuppi = cms.EDProducer("PatJetIDValueMapProducer",
        filterParams = cms.PSet(
            quality = cms.string("TIGHTLEPVETO"),
            version = cms.string("RUN2ULPUPPI"),
        ),
        src = cms.InputTag(inputJets)
    )

    process.tightjetidpuppi = cms.EDProducer("PatJetIDValueMapProducer",
        filterParams = cms.PSet(
            quality = cms.string("TIGHT"),
            version = cms.string("RUN2ULPUPPI")
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
            jetIdTight = cms.InputTag("tightjetidpuppi"),
        ) 
    )

    process.BigPuppiJetTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
        src = cms.InputTag('updatedJetsPuppi'),
        cut = cms.string(''),
        name = cms.string('updatedJetsPuppi'),
        singleton = cms.bool(False),
        extension = cms.bool(False),
        externalVariables = cms.PSet(), #add btag here?
        variables = cms.PSet(
            pt = Var("pt()", float, doc="pT of AK4 jet",precision=-1),
            eta = Var("eta()", float, doc="eta of AK4 jet",precision=-1),
            phi = Var("phi()", float, doc="phi of AK4 jet",precision=-1),
            area = Var("jetArea()", float, doc="jet catchment area, for JECs",precision=-1),
            jetIdLepVeto = Var("userInt('jetIdLepVeto')", int),
            jetIdTight = Var("userInt('jetIdTight')", int),
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

    process.UpdatedPuppiJetsTask = cms.Task(
        process.jetIdLepVetoPuppi,
        process.tightjetidpuppi,
        process.pileupJetIdPuppi,
        process.updatedJetsPuppi,
        process.BigPuppiJetTable
    )

    process.schedule.associate(process.UpdatedPuppiJetsTask) 

    if isMC:
        process.MCUpdatedPuppiJetsTask = cms.Task(
            process.selectedGenJets,
        )
        process.schedule.associate(process.MCUpdatedPuppiJetsTask) 


    return process

