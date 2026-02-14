from re import split
import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.LundDeltaPsiProducer_cfi import LundDeltaPsiProducer
from PhysicsTools.NanoAOD.common_cff import *


def _createDeltaPsiTable(deltapsi, tablename):
    """Create a DeltaPsi table producer.
    
    Args:
        jetsname: The name of the jets collection
        hard_side: Boolean indicating whether to use the hard side or soft side
    
    Returns:
        A cms.EDProducer configured for the DeltaPsi table
    """
    return cms.EDProducer("SimpleSplittingInfoFlatTableProducer",
        src = cms.InputTag(deltapsi),
        cut = cms.string(""),
        name = cms.string(tablename),
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
    )

def addLundDeltaPsi(process, jetsname):
    # Add LundDeltaPsiProducer
    setattr(process, jetsname+"HardSideDeltaPsiProducer", LundDeltaPsiProducer.clone(
        src = cms.InputTag(jetsname),
        hardSide = cms.bool(True),
    ))
    setattr(process, jetsname+"SoftSideDeltaPsiProducer", LundDeltaPsiProducer.clone(
        src = cms.InputTag(jetsname),
        hardSide = cms.bool(False),
    ))
    setattr(process, jetsname+'HardSideDeltaPsiTable', _createDeltaPsiTable(jetsname+"HardSideDeltaPsiProducer", jetsname+'HardSideDeltaPsiTable'))
    setattr(process, jetsname+'SoftSideDeltaPsiTable', _createDeltaPsiTable(jetsname+"SoftSideDeltaPsiProducer", jetsname+'SoftSideDeltaPsiTable'))
    
    setattr(process, jetsname+"DeltaPsiTask", cms.Task(
        getattr(process, jetsname+"HardSideDeltaPsiProducer"),
        getattr(process, jetsname+'HardSideDeltaPsiTable'),
        getattr(process, jetsname+"SoftSideDeltaPsiProducer"),
        getattr(process, jetsname+'SoftSideDeltaPsiTable')
    ))
    process.schedule.associate(getattr(process, jetsname+"DeltaPsiTask"))    
    
    return process

def addGenDeltaPsi(process, genParticles, genJets):
    process.GenHardSideDeltaPsiProducer = cms.EDProducer("GenShowerSplittingsProducer",
        src = cms.InputTag("genParticles"),
        jets = cms.InputTag('arbitratedGenJetsAK8'),
        verbose = cms.int32(0),
        hardSide = cms.bool(True)
    )
    process.GenSoftSideDeltaPsiProducer = cms.EDProducer("GenShowerSplittingsProducer",
        src = cms.InputTag("genParticles"),
        jets = cms.InputTag('arbitratedGenJetsAK8'),
        verbose = cms.int32(0),
        hardSide = cms.bool(False)
    )

    process.GenHardSideDeltaPsiTable = _createDeltaPsiTable('GenHardSideDeltaPsiProducer', 'GenHardSideDeltaPsiTable')
    process.GenSoftSideDeltaPsiTable = _createDeltaPsiTable('GenSoftSideDeltaPsiProducer', 'GenSoftSideDeltaPsiTable')

    process.GenDeltaPsiTask = cms.Task(
        process.GenHardSideDeltaPsiProducer,
        process.GenHardSideDeltaPsiTable,
        process.GenSoftSideDeltaPsiProducer,
        process.GenSoftSideDeltaPsiTable
    )
    process.schedule.associate(process.GenDeltaPsiTask)
    
    return process

