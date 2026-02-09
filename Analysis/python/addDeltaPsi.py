import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.DeltaPsiProducer_cfi import DeltaPsiProducer
from PhysicsTools.NanoAOD.common_cff import *


def addDeltaPsi(process, jetsname):
    # Add DeltaPsiProducer
    setattr(process, jetsname+"DeltaPsiProducer", DeltaPsiProducer.clone(
        src = cms.InputTag(jetsname)
    ))
    setattr(process, jetsname+'DeltaPsiTable', cms.EDProducer("SimpleSplittingInfoFlatTableProducer",
        src = cms.InputTag(jetsname+"DeltaPsiProducer"),
        cut = cms.string(""),
        name = cms.string(jetsname+'BK'),
        extension=cms.bool(True),
        singleton = cms.bool(False),
        variables = cms.PSet(
            deltaPsi = Var("deltaPsi", float, doc="Delta Psi of the hardest splitting", precision=-1),
        ),
    ))
    
    setattr(process, jetsname+"DeltaPsiTask", cms.Task(
        getattr(process, jetsname+"DeltaPsiProducer"),
        getattr(process, jetsname+'DeltaPsiTable')
    ))
    process.schedule.associate(getattr(process, jetsname+"DeltaPsiTask"))
    
    
    #getattr(process, jetsname+"Table").extraFloatNames.append("DeltaPsi")
    #getattr(process, jetsname+"Table").extraFloats.append(cms.InputTag(jetsname+"DeltaPsiProducer"))
    
    
    return process