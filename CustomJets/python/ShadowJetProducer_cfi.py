import FWCore.ParameterSet.Config as cms

RecoShadowJetProducer = cms.EDProducer("RecoShadowJetProducer",
    minPartPt = cms.double(1e-3),
    maxNumPart = cms.uint32(512),
    dRwindow = cms.double(0.2),
    onlyFromPV = cms.bool(False),
    onlyCharged = cms.bool(False),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("pupppi"),
    jetSrc = cms.InputTag("GenSimonJets"),

    verbose = cms.int32(0)
)

GenShadowJetProducer = cms.EDProducer("GenShadowJetProducer",
    minPartPt = cms.double(1e-3),
    maxNumPart = cms.uint32(512),
    dRwindow = cms.double(0.2),
    onlyFromPV = cms.bool(False),
    onlyCharged = cms.bool(False),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("genParticles"),
    jetSrc = cms.InputTag("SimonJets"),

    verbose = cms.int32(0)
)

CandidateShadowJetProducer = cms.EDProducer("CandidateShadowJetProducer",
    minPartPt = cms.double(1e-3),
    maxNumPart = cms.uint32(512),
    dRwindow = cms.double(0.2),
    onlyFromPV = cms.bool(False),
    onlyCharged = cms.bool(False),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("genParticles"),
    jetSrc = cms.InputTag("SimonJets"),

    verbose = cms.int32(0)
)
