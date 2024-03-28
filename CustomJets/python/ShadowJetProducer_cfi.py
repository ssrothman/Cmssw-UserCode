import FWCore.ParameterSet.Config as cms
from SRothman.CustomJets.cuts_cff import *
from SRothman.CustomJets.systematics import *

RecoShadowJetProducer = cms.EDProducer("RecoShadowJetProducer",
    systematics = systematics.clone(),
    syst = syst,

    thresholds = recoThresholds.clone(),
    vtxCuts = recoVtxCuts.clone(),

    onlyCharged = cms.bool(False),

    maxNumPart = cms.uint32(512),
    minNumPart = cms.uint32(2),

    dRwindow = cms.double(0.4),

    applyPuppi = cms.bool(True),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("pupppi"),
    jetSrc = cms.InputTag("GenSimonJets"),

    verbose = cms.int32(0)
)

GenShadowJetProducer = cms.EDProducer("CandidateShadowJetProducer",
    systematics = systematics.clone(),
    syst = cms.string("NOM"),

    thresholds = genThresholds.clone(),
    vtxCuts = genVtxCuts.clone(),

    maxNumPart = cms.uint32(512),
    minNumPart = cms.uint32(2),

    dRwindow = cms.double(0.4),

    applyPuppi = cms.bool(True),

    onlyCharged = cms.bool(False),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("genParticles"),
    jetSrc = cms.InputTag("SimonJets"),

    verbose = cms.int32(0)
)

'''
CandidateShadowJetProducer = cms.EDProducer("CandidateShadowJetProducer",
    thresholds = recoThresholds.clone(),
    vtxCuts = recoVtxCuts.clone(),

    maxNumPart = cms.uint32(512),
    minNumPart = cms.uint32(2),

    dRwindow = cms.double(0.4),

    applyPuppi = cms.bool(True),

    onlyCharged = cms.bool(False),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("genParticles"),
    jetSrc = cms.InputTag("SimonJets"),

    verbose = cms.int32(0)
)
'''
