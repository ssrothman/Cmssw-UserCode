import FWCore.ParameterSet.Config as cms

DeltaPsiProducer = cms.EDProducer('DeltaPsiProducer',
    src = cms.InputTag(''),  # Input simon::jet collection
    verbose = cms.int32(0)
)
