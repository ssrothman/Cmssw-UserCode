import FWCore.ParameterSet.Config as cms
from SRothman.CustomJets.thresholds_cff import *

RecoShadowJetProducer = cms.EDProducer("RecoShadowJetProducer",
    thresholds = recoThresholds.clone(),

    onlyFromPV = cms.bool(False),
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

GenShadowJetProducer = cms.EDProducer("GenShadowJetProducer",
    thresholds = genThresholds.clone(),

    maxNumPart = cms.uint32(512),
    minNumPart = cms.uint32(0),

    dRwindow = cms.double(0.4),

    applyPuppi = cms.bool(True),

    onlyFromPV = cms.bool(False),
    onlyCharged = cms.bool(False),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("genParticles"),
    jetSrc = cms.InputTag("SimonJets"),

    verbose = cms.int32(0)
)

CandidateShadowJetProducer = cms.EDProducer("CandidateShadowJetProducer",
    thresholds = recoThresholds.clone(),

    maxNumPart = cms.uint32(512),
    minNumPart = cms.uint32(2),

    dRwindow = cms.double(0.4),

    applyPuppi = cms.bool(True),

    onlyFromPV = cms.bool(False),
    onlyCharged = cms.bool(False),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("genParticles"),
    jetSrc = cms.InputTag("SimonJets"),

    verbose = cms.int32(0)
)
