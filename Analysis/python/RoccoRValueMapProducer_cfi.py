import FWCore.ParameterSet.Config as cms

RoccoRValueMapProducer = cms.EDProducer('RoccoRValueMapProducer',
    src = cms.InputTag('linkedObjects', 'muons'),
    genParticles = cms.InputTag('prunedGenParticles'),
    isMC = cms.bool(True),
    dataFile = cms.FileInPath(""),
    verbose = cms.int32(0)
)

from Configuration.Eras.Era_Run2_2017_cff import Run2_2017
from Configuration.Eras.Era_Run2_2018_cff import Run2_2018

Run2_2017.toModify(RoccoRValueMapProducer, dataFile="SRothman/Analysis/data/RoccoR2017UL.txt")
Run2_2018.toModify(RoccoRValueMapProducer, dataFile="SRothman/Analysis/data/RoccoR2018UL.txt")
