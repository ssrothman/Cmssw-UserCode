import FWCore.ParameterSet.Config as cms

from SRothman.CustomJets.thresholds_cff import *

_maxNumPart = 4096

PatSimonJetProducer = cms.EDProducer("PatSimonJetProducer",
    jetSrc = cms.InputTag("selectedPatJets"),

    thresholds = recoThresholds.clone(),

    maxNumPart = cms.uint32(_maxNumPart),
    minNumPart = cms.uint32(2),

    onlyFromPV = cms.bool(False),
    onlyCharged = cms.bool(False),

    applyJEC=cms.bool(False),
    applyPuppi = cms.bool(True),

    minPt = cms.double(20),
    maxEta = cms.double(2.1),

    maxMuFrac = cms.double(0.8),
    maxChEmFrac = cms.double(0.8),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    CHSsrc = cms.InputTag(""),
    addCHSindex = cms.bool(False),
    CHSmatchDR = cms.double(0.2),

    verbose = cms.int32(1),
)

GenSimonJetProducer = cms.EDProducer("GenSimonJetProducer",
    jetSrc = cms.InputTag("ak4GenJetsNoNu"),

    thresholds = genThresholds.clone(),

    maxNumPart = cms.uint32(_maxNumPart),
    minNumPart = cms.uint32(0),

    onlyFromPV = cms.bool(False),
    onlyCharged = cms.bool(False),

    applyJEC=cms.bool(False),
    applyPuppi = cms.bool(False),

    minPt = cms.double(10),
    maxEta = cms.double(2.5),

    maxMuFrac = cms.double(999),
    maxChEmFrac = cms.double(999),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    CHSsrc = cms.InputTag(""),
    addCHSindex = cms.bool(False),
    CHSmatchDR = cms.double(0.4),

    verbose = cms.int32(1),
)
