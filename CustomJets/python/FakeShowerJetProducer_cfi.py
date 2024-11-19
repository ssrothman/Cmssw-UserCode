import FWCore.ParameterSet.Config as cms
from SRothman.CustomJets.SimonJetProducer_cfi import *

PatFakeShowerJetProducer = cms.EDProducer("PatFakeShowerJetProducer",
    jetSrc = PatSimonJetProducer.jetSrc,

    maxNumPart = PatSimonJetProducer.maxNumPart,
    minNumPart = PatSimonJetProducer.minNumPart,

    minPt = PatSimonJetProducer.minPt,
    maxEta = PatSimonJetProducer.maxEta,

    maxMuFrac = PatSimonJetProducer.maxMuFrac,
    maxChEmFrac = PatSimonJetProducer.maxChEmFrac,

    verbose = PatSimonJetProducer.verbose,

    phi_mode = cms.string("UNIFORM"),
    z_mode = cms.string("GLUON"),
    theta_mode = cms.string("LNX"),
    zcut = cms.double(0.01),
    theta_min = cms.double(0.01),
    theta_max = cms.double(0.5),
)

GenFakeShowerJetProducer = cms.EDProducer("GenFakeShowerJetProducer",
    jetSrc = GenSimonJetProducer.jetSrc,

    maxNumPart = GenSimonJetProducer.maxNumPart,
    minNumPart = GenSimonJetProducer.minNumPart,

    minPt = GenSimonJetProducer.minPt,
    maxEta = GenSimonJetProducer.maxEta,

    maxMuFrac = GenSimonJetProducer.maxMuFrac,
    maxChEmFrac = GenSimonJetProducer.maxChEmFrac,

    verbose = GenSimonJetProducer.verbose,

    phi_mode = cms.string("UNIFORM"),
    z_mode = cms.string("GLUON"),
    theta_mode = cms.string("LNX"),
    zcut = cms.double(0.01),
    theta_min = cms.double(0.01),
    theta_max = cms.double(0.5),
)
