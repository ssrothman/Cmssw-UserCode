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

    CHSsrc = cms.InputTag(""),
    addCHSindex = cms.bool(False),
    CHSmatchDR = cms.double(0.4),

    verbose = cms.int32(1),
)
