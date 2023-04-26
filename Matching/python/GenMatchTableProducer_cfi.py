import FWCore.ParameterSet.Config as cms

GenMatchTableProducer = cms.EDProducer("GenMatchTableProducer",
    src = cms.InputTag("GenMatchProducer"),
    name = cms.string("GenMatch")
)
