import FWCore.ParameterSet.Config as cms

from SRothman.CustomJets.systematics import *

_maxNumPart = 4096

from SRothman.Analysis.config.config import config

PatEventJetProducer = cms.EDProducer("PatEventJetProducer",
    selector = cms.PSet(
        parameters = systematics_parameters,
        settings = NOM,
    ),

    jetSrc = cms.InputTag("selectedPatJets"),
    pfCandidates = cms.InputTag("packedPFCandidates"),
    CHSsrc = cms.InputTag(""),
    addCHSindex = cms.bool(False),
    CHSmatchDR = cms.double(0.4),

    verbose = cms.int32(1),
)

GenEventJetProducer = cms.EDProducer("GenEventJetProducer",
    selector = cms.PSet(
        parameters = systematics_parameters,
        settings = NOM,
    ),

    jetSrc = cms.InputTag("ak4GenJetsNoNu"),
    pfCandidates = cms.InputTag("packedPFCandidates"),

    CHSsrc = cms.InputTag(""),
    addCHSindex = cms.bool(False),
    CHSmatchDR = cms.double(0.4),

    verbose = cms.int32(1),
)
