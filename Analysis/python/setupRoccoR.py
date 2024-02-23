import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.RoccoRValueMapProducer_cfi import RoccoRValueMapProducer

def setupRoccoR(process, isMC=True):
    process.RoccoR = RoccoRValueMapProducer.clone(
        src = cms.InputTag('linkedObjects', 'muons'),
        genParticles = cms.InputTag('packedGenParticles'),
        isMC = cms.bool(isMC),
        verbose = False
    )
    process.RoccoRTask = cms.Task(process.RoccoR)
    process.schedule.associate(process.RoccoRTask)

    process.muonTable.externalVariables.RoccoR = cms.PSet(
        compression = cms.string('none'),
        doc = cms.string("Rochester correction factor"),
        mcOnly = cms.bool(False),
        precision = cms.int32(-1),
        src = cms.InputTag("RoccoR"),
        type = cms.string('float')
    )
    return process
