import FWCore.ParameterSet.Config as cms
from SRothman.CustomJets.SimonJetProducer_cfi import *

PatUniformGaussianJetProducer = cms.EDProducer("PatUniformGaussianJetProducer",
    systematics = PatSimonJetProducer.systematics.clone(),
    syst = PatSimonJetProducer.syst,

    jetSrc = PatSimonJetProducer.jetSrc,
    thresholds = PatSimonJetProducer.thresholds,
    vtxCuts = PatSimonJetProducer.vtxCuts,

    maxNumPart = PatSimonJetProducer.maxNumPart,
    minNumPart = PatSimonJetProducer.minNumPart,

    onlyCharged = PatSimonJetProducer.onlyCharged,

    applyJEC=PatSimonJetProducer.applyJEC,
    applyPuppi = PatSimonJetProducer.applyPuppi,

    minPt = PatSimonJetProducer.minPt,
    maxEta = PatSimonJetProducer.maxEta,

    maxMuFrac = PatSimonJetProducer.maxMuFrac,
    maxChEmFrac = PatSimonJetProducer.maxChEmFrac,

    eventSelection = PatSimonJetProducer.eventSelection,
    doEventSelection = PatSimonJetProducer.doEventSelection,

    CHSsrc = PatSimonJetProducer.CHSsrc,
    addCHSindex = PatSimonJetProducer.addCHSindex,
    CHSmatchDR = PatSimonJetProducer.CHSmatchDR,

    verbose = PatSimonJetProducer.verbose,

    radius = cms.double(0.4)
)

GenUniformGaussianJetProducer = cms.EDProducer("GenUniformGaussianJetProducer",
    systematics = GenSimonJetProducer.systematics.clone(),
    syst = GenSimonJetProducer.syst,

    jetSrc = GenSimonJetProducer.jetSrc,

    thresholds = GenSimonJetProducer.thresholds,
    vtxCuts = GenSimonJetProducer.vtxCuts,

    maxNumPart = GenSimonJetProducer.maxNumPart,
    minNumPart = GenSimonJetProducer.minNumPart,

    onlyCharged = GenSimonJetProducer.onlyCharged,

    applyJEC=GenSimonJetProducer.applyJEC,
    applyPuppi = GenSimonJetProducer.applyPuppi,

    minPt = GenSimonJetProducer.minPt,
    maxEta = GenSimonJetProducer.maxEta,

    maxMuFrac = GenSimonJetProducer.maxMuFrac,
    maxChEmFrac = GenSimonJetProducer.maxChEmFrac,

    eventSelection = GenSimonJetProducer.eventSelection,
    doEventSelection = GenSimonJetProducer.doEventSelection,

    CHSsrc = GenSimonJetProducer.CHSsrc,
    addCHSindex = GenSimonJetProducer.addCHSindex,
    CHSmatchDR = GenSimonJetProducer.CHSmatchDR,

    verbose = GenSimonJetProducer.verbose,

    radius = cms.double(0.4)
)
