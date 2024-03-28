import FWCore.ParameterSet.Config as cms
from SRothman.CustomJets.cuts_cff import *
from SRothman.CustomJets.systematics import *

_maxNumPart = 8192

RecoFullEventJetProducer = cms.EDProducer("FullEventJetProducer",
    systematics = systematics.clone(),
    syst = syst,

    thresholds = recoThresholds.clone(),
    vtxCuts = recoVtxCuts.clone(),

    onlyCharged = cms.bool(False),

    maxNumPart = cms.uint32(_maxNumPart),
    minNumPart = cms.uint32(2),

    maxPartEta = cms.double(2.5),

    applyPuppi = cms.bool(True),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("pupppi"),

    skipLeadingMuons = cms.bool(True),

    verbose = cms.int32(0)
)

GenFullEventJetProducer = cms.EDProducer("FullEventJetProducer",
    systematics = systematics.clone(),
    syst = cms.string("NOM"),

    thresholds = genThresholds.clone(),
    vtxCuts = genVtxCuts.clone(),

    onlyCharged = cms.bool(False),

    maxNumPart = cms.uint32(_maxNumPart),
    minNumPart = cms.uint32(2),

    maxPartEta = cms.double(2.5),

    applyPuppi = cms.bool(False),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("packedGenParticles"),

    skipLeadingMuons = cms.bool(True),
    verbose = cms.int32(0)
)
