import FWCore.ParameterSet.Config as cms

LundDeltaPsiProducer = cms.EDProducer('LundDeltaPsiProducer',
    src = cms.InputTag(''),  # Input simon::jet collection
    verbose = cms.int32(0),
    hardSide = cms.bool(True),  # Whether to compute for the hard side or soft side splitting
    zcut1 = cms.double(0.1),  # Minimum z for the first splitting
    zcut2 = cms.double(0.2),  # Minimum z for the second splitting
)