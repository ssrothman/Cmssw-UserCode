import FWCore.ParameterSet.Config as cms

RecoFullEventJetProducer = cms.EDProducer("RecoFullEventJetProducer",
    minPartPt = cms.double(1e-3),
    maxNumPart = cms.uint32(512),
    maxEta = cms.double(2.7),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("pupppi"),

    verbose = cms.int32(0)
)

GenFullEventJetProducer = cms.EDProducer("GenFullEventJetProducer",
    minPartPt = cms.double(1e-3),
    maxNumPart = cms.uint32(512),
    maxEta = cms.double(2.7),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("genParticles"),

    verbose = cms.int32(0)
)

CandidateFullEventJetProducer = cms.EDProducer("CandidateFullEventJetProducer",
    minPartPt = cms.double(1e-3),
    maxNumPart = cms.uint32(512),
    maxEta = cms.double(2.7),

    eventSelection = cms.InputTag(""),
    doEventSelection = cms.bool(False),

    partSrc = cms.InputTag("genParticles"),

    verbose = cms.int32(0)
)
