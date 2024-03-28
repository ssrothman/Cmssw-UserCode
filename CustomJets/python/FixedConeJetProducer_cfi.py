import FWCore.ParameterSet.Config as cms

from SRothman.CustomJets.cuts_cff import *
from SRothman.CustomJets.systematics import *

_maxNumPart = 8192

RecoFixedConeJetProducer = cms.EDProducer("FixedConeJetProducer",
    systematics = systematics.clone(),
    syst = syst,

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
    systematics = systematics.clone(),
    syst = cms.string("NOM"),

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
