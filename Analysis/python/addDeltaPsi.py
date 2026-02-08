import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.DeltaPsiProducer_cfi import DeltaPsiProducer

def addDeltaPsi(process, jetsname):
    # Add DeltaPsiProducer
    setattr(process, jetsname+"DeltaPsiProducer", DeltaPsiProducer.clone(
        src = cms.InputTag(jetsname)
    ))
    setattr(process, jetsname+"DeltaPsiTask", cms.Task(getattr(process, jetsname+"DeltaPsiProducer")))
    process.schedule.associate(getattr(process, jetsname+"DeltaPsiTask"))
    getattr(process, jetsname+"Table").extraFloatNames.append("DeltaPsi")
    getattr(process, jetsname+"Table").extraFloats.append(cms.InputTag(jetsname+"DeltaPsiProducer"))
    return process