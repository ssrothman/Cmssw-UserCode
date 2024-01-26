import FWCore.ParameterSet.Config as cms
from SRothman.CustomJets.cuts_cff import *

FullEventJetProducer = cms.EDProducer("FullEventJetProducer",
    thresholds = recoThresholds.clone(),
    vtxCuts = recoVtxCuts.clone(),

    onlyCharged = cms.bool(False),

    maxNumPart = cms.uint32(2048),

    maxPartEta = cms.double(2.4),

    applyPuppi = cms.bool(True),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("pupppi"),

    verbose = cms.int32(0)
)
