import FWCore.ParameterSet.Config as cms

ResolutionStudyTable = cms.EDProducer('ResolutionStudyTableProducer',
    name = cms.string("ResolutionStudy"),
    reco = cms.InputTag("SimonJets"),    
    gen = cms.InputTag("GenSimonJets"),
    src = cms.InputTag("RuleMatchProducer"),
    verbose = cms.int32(0)
)
