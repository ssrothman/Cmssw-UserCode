import FWCore.ParameterSet.Config as cms

SVMatchTableProducer = cms.EDProducer("SVMatchTableProducer",
    name = cms.string("SVMatch"),
    jets = cms.InputTag("linkedObjects", "jets"),
    SVs = cms.InputTag("vertexTable"),
    DRthresh = cms.double(0.4),
)
