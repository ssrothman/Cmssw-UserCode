from re import split
import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.DeltaPsiProducer_cfi import DeltaPsiProducer
from PhysicsTools.NanoAOD.common_cff import *


def addDeltaPsi(process, jetsname):
    # Add DeltaPsiProducer
    setattr(process, jetsname+"HardSideDeltaPsiProducer", DeltaPsiProducer.clone(
        src = cms.InputTag(jetsname),
        hardSide = cms.bool(True),
        zcut1 = cms.double(0.1),
        zcut2 = cms.double(0.2)
    ))
    setattr(process, jetsname+"SoftSideDeltaPsiProducer", DeltaPsiProducer.clone(
        src = cms.InputTag(jetsname),
        hardSide = cms.bool(False),
        zcut1 = cms.double(0.1),
        zcut2 = cms.double(0.2)
    ))
    setattr(process, jetsname+'HardSideDeltaPsiTable', cms.EDProducer("SimpleSplittingInfoFlatTableProducer",
        src = cms.InputTag(jetsname+"HardSideDeltaPsiProducer"),
        cut = cms.string(""),
        name = cms.string('DeltaPsiHardSide'),
        extension=cms.bool(False),
        singleton = cms.bool(False),
        variables = cms.PSet(
            pt1 = Var("split123.p1.pt()", float, precision=-1),
            pt4 = Var("split456.p1.pt()", float, precision=-1),
            pdgId1 = Var("split123.pdgId1", int),
            pdgId2 = Var("split123.pdgId2", int),
            pdgId3 = Var("split123.pdgId3", int),
            pdgId4 = Var("split456.pdgId1", int),
            pdgId5 = Var("split456.pdgId2", int),
            pdgId6 = Var("split456.pdgId3", int),
            deltaR23 = Var('split123.deltaR', float, precision=-1),
            deltaR56 = Var('split456.deltaR', float, precision=-1),
            z23 = Var('split123.z', float, precision=-1),
            z56 = Var('split456.z', float, precision=-1),
            kt23 = Var('split123.kt', float, precision=-1),
            kt56 = Var('split456.kt', float, precision=-1),
            deltaPsi_type1 = Var('deltaPsi_type1()', float, precision=-1),
            deltaPsi_type2 = Var('deltaPsi_type2()', float, precision=-1),
            deltaPsi_type3 = Var('deltaPsi_type3()', float, precision=-1),
            deltaPsi_type4 = Var('deltaPsi_type4()', float, precision=-1),
            psi123_type1 = Var('split123.psi_type1()', float, precision=-1),
            psi456_type1 = Var('split456.psi_type1()', float, precision=-1),
        ),
    ))
    setattr(process, jetsname+'SoftSideDeltaPsiTable', cms.EDProducer("SimpleSplittingInfoFlatTableProducer",
        src = cms.InputTag(jetsname+"SoftSideDeltaPsiProducer"),
        cut = cms.string(""),
        name = cms.string('DeltaPsiSoftSide'),
        extension=cms.bool(False),
        singleton = cms.bool(False),
        variables = cms.PSet(
            pt1 = Var("split123.p1.pt()", float, precision=-1),
            pt4 = Var("split456.p1.pt()", float, precision=-1),
            pdgId1 = Var("split123.pdgId1", int),
            pdgId2 = Var("split123.pdgId2", int),
            pdgId3 = Var("split123.pdgId3", int),
            pdgId4 = Var("split456.pdgId1", int),
            pdgId5 = Var("split456.pdgId2", int),
            pdgId6 = Var("split456.pdgId3", int),
            deltaR23 = Var('split123.deltaR', float, precision=-1),
            deltaR56 = Var('split456.deltaR', float, precision=-1),
            z23 = Var('1-split123.z', float, precision=-1),
            z56 = Var('split456.z', float, precision=-1),
            kt23 = Var('split123.kt', float, precision=-1),
            kt56 = Var('split456.kt', float, precision=-1),
            deltaPsi_type1 = Var('deltaPsi_type1()', float, precision=-1),
            deltaPsi_type2 = Var('deltaPsi_type2()', float, precision=-1),
            deltaPsi_type3 = Var('deltaPsi_type3()', float, precision=-1),
            deltaPsi_type4 = Var('deltaPsi_type4()', float, precision=-1),
            psi123_type1 = Var('split123.psi_type1()', float, precision=-1),
            psi456_type1 = Var('split456.psi_type1()', float, precision=-1),
        ),
    ))

    
    setattr(process, jetsname+"DeltaPsiTask", cms.Task(
        getattr(process, jetsname+"HardSideDeltaPsiProducer"),
        getattr(process, jetsname+'HardSideDeltaPsiTable'),
        getattr(process, jetsname+"SoftSideDeltaPsiProducer"),
        getattr(process, jetsname+'SoftSideDeltaPsiTable')
    ))
    process.schedule.associate(getattr(process, jetsname+"DeltaPsiTask"))
    
    #getattr(process, jetsname+"Table").extraFloatNames.append("DeltaPsi")
    #getattr(process, jetsname+"Table").extraFloats.append(cms.InputTag(jetsname+"DeltaPsiProducer"))
    
    
    return process