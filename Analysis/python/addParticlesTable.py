import FWCore.ParameterSet.Config as cms
from PhysicsTools.NanoAOD.common_cff import *

def addParticlesTable(process, eventSelection, name, singleton=False):
    setattr(process, name+"particlesTable", cms.EDProducer("SimpleCandidateFlatTableProducer",
        src = cms.InputTag(eventSelection),
        cut = cms.string(""),
        name = cms.string(name),
        doc = cms.string("Basic candidate table"),
        singleton=cms.bool(singleton), 
        extension=cms.bool(False),
        variables=cms.PSet(
            pt = Var("pt", float, doc="pt", precision=-1),
            eta = Var("eta", float, doc="eta", precision=-1),
            phi = Var("phi", float, doc="phi", precision=-1),
            mass = Var("mass", float, doc="mass", precision=-1),
            pdgId = Var("pdgId", int, doc="pdgId", precision=-1),
            charge = Var("charge", int, doc="charge", precision=-1),
        )))

    setattr(process, name+'particlesTableTask', cms.Task(getattr(process, name+'particlesTable')))
    process.schedule.associate(getattr(process, name+'particlesTableTask'))
    return process
