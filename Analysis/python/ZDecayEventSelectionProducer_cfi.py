import FWCore.ParameterSet.Config as cms

ZDecayEventSelectionProducer = cms.EDFilter('ZDecayEventSelectionProducer',
    src = cms.InputTag('genParticles'),
    leadPt = cms.double(20.0),
    subPt = cms.double(10.0),
    maxEta = cms.double(3.0),
    minZmass = cms.double(65.0),
    maxZmass = cms.double(115.0),
    oppositeSign = cms.bool(True),
    pdgIds = cms.vint32([13]),
    verbose = cms.int32(0),
    onlyCheckLeading = cms.bool(False),
    saveParticles = cms.bool(True),
)
