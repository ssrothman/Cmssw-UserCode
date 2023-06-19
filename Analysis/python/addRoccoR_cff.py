import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.RoccoRValueMapProducer_cfi import RoccoRValueMapProducer

def addRoccoR(process, verbose=False):
    process.RoccoR = RoccoRValueMapProducer.clone(
        src = cms.InputTag('linkedObjects', 'muons'),
        genParticles = cms.InputTag('prunedGenParticles'),
        isMC = cms.bool(True),
        dataFile = cms.FileInPath('SRothman/Analysis/data/RoccoR2017UL.txt'),
        verbose = verbose
    )
    process.RoccoRTask = cms.Task(process.RoccoR)
    process.schedule.associate(process.RoccoRTask)
    process.muonTable.externalVariables.RoccoR = cms.PSet(
        compression = cms.string('none'),
        doc = cms.string("Rochester correction factor"),
        mcOnly = cms.bool(False),
        precision = cms.int32(10),
        src = cms.InputTag("RoccoR"),
        type = cms.string('float')
    )
    return process
