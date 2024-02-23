import FWCore.ParameterSet.Config as cms

from SRothman.CustomJets.cuts_cff import *

_maxNumPart = 8192

RecoFixedConeJetProducer = cms.EDProducer("FixedConeJetProducer",
    thresholds = recoThresholds.clone(),
    vtxCuts = recoVtxCuts.clone(),

    maxNumPart = cms.uint32(_maxNumPart),
    minNumPart = cms.uint32(2),

    onlyCharged = cms.bool(False),
    applyPuppi = cms.bool(True),

    particles = cms.InputTag("packedPFCandidates"),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    coords = cms.InputTag("randomConeCoords"),

    conesize = cms.double(0.4),

    verbose = cms.int32(0),
)

GenFixedConeJetProducer = cms.EDProducer("FixedConeJetProducer",
    thresholds = genThresholds.clone(),
    vtxCuts = genVtxCuts.clone(),

    maxNumPart = cms.uint32(_maxNumPart),
    minNumPart = cms.uint32(2),

    onlyCharged = cms.bool(False),
    applyPuppi = cms.bool(False),

    particles = cms.InputTag("packedGenParticles"),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    coords = cms.InputTag("randomConeCoords"),

    conesize = cms.double(0.4),

    verbose = cms.int32(0),
)
