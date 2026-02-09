from re import split
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
            splitting_pt1 = Var("pt1", float, precision=-1),
            splitting_pt2 = Var("pt2", float, precision=-1),
            splitting_pt3 = Var("pt3", float, precision=-1),
            splitting_pt4 = Var("pt4", float, precision=-1),
            splitting_pt5 = Var("pt5", float, precision=-1),
            splitting_dR23 = Var('delta_R23', float, precision=-1),
            splitting_z23 = Var('z23', float, precision=-1),
            splitting_kt23 = Var('kt23', float, precision=-1),
            splitting_phi23 = Var('phi23', float, precision=-1),
            splitting_dR45 = Var('delta_R45', float, precision=-1),
            splitting_z45 = Var('z45', float, precision=-1),
            splitting_kt45 = Var('kt45', float, precision=-1),
            splitting_phi45 = Var('phi45', float, precision=-1),
            splitting_deltaPsi = Var('deltaPsi', float, precision=-1)
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