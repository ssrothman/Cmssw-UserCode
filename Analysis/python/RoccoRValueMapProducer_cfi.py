import FWCore.ParameterSet.Config as cms

RoccoRValueMapProducer = cms.EDProducer('RoccoRValueMapProducer',
    src = cms.InputTag('linkedObjects', 'muons'),
    genParticles = cms.InputTag('prunedGenParticles'),
    isMC = cms.bool(True),
    dataFile = cms.FileInPath('SRothman/Analysis/data/RoccoR2017UL.txt'),
)
