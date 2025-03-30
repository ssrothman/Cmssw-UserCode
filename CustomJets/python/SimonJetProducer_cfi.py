import FWCore.ParameterSet.Config as cms

from SRothman.CustomJets.cuts_cff import *
from SRothman.CustomJets.systematics import *

_maxNumPart = 4096

from SRothman.Analysis.config.config import config

PatSimonJetProducer = cms.EDProducer("PatSimonJetProducer",
    selector = cms.PSet(
        parameters = systematics_parameters,
        settings = NOM,
    ),

    jetSrc = cms.InputTag("selectedPatJets"),

    thresholds = recoThresholds.clone(),
    vtxCuts = recoVtxCuts.clone(),

    maxNumPart = cms.uint32(_maxNumPart),
    minNumPart = cms.uint32(0),

    onlyCharged = cms.bool(False),

    applyJEC=cms.bool(False),
    applyPuppi = cms.bool(True),

    minPt = cms.double(0),
    maxEta = cms.double(999),

    maxMuFrac = cms.double(999),
    maxChEmFrac = cms.double(999),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    CHSsrc = cms.InputTag(""),
    addCHSindex = cms.bool(False),
    CHSmatchDR = cms.double(0.4),

    verbose = cms.int32(1),
)

GenSimonJetProducer = cms.EDProducer("GenSimonJetProducer",
    selector = cms.PSet(
        parameters = systematics_parameters,
        settings = NOM,
    ),

    jetSrc = cms.InputTag("ak4GenJetsNoNu"),

    thresholds = genThresholds.clone(),
    vtxCuts = genVtxCuts.clone(),

    maxNumPart = cms.uint32(_maxNumPart),
    minNumPart = cms.uint32(0),

    onlyCharged = cms.bool(False),

    applyJEC=cms.bool(False),
    applyPuppi = cms.bool(False),

    minPt = cms.double(0),
    maxEta = cms.double(999),

    maxMuFrac = cms.double(999),
    maxChEmFrac = cms.double(999),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    CHSsrc = cms.InputTag(""),
    addCHSindex = cms.bool(False),
    CHSmatchDR = cms.double(0.4),

    verbose = cms.int32(1),
)
