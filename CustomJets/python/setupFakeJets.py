import FWCore.ParameterSet.Config as cms
from SRothman.CustomJets.SimonJetTableProducer_cfi import *
from SRothman.CustomJets.FakeShowerJetProducer_cfi import *

def setupGenFakeShowerJets(process,
                     genjets,
                     name,
                     phimodulation):

    setattr(process, 'Gen'+name, GenFakeShowerJetProducer.clone(
        jetSrc = genjets,
        verbose = False,
        phi_mode = "COS2PHI" if phimodulation else "UNIFORM",
    ))

    setattr(process, 'Gen'+name+'Table', SimonJetTableProducer.clone(
        src = 'Gen'+name,
        name = 'Gen'+name,
        verbose=False,
        isGen = True
    ))

    setattr(process, name+"MCTask", cms.Task(
        getattr(process, 'Gen'+name),
        getattr(process, 'Gen'+name+'Table')
    ))
    process.schedule.associate(getattr(process, name+'MCTask'))

    return process

def setupRecoFakeShowerJets(process,
                       jets,
                       name,
                       phimodulation):
    setattr(process, name, PatFakeShowerJetProducer.clone(
        jetSrc = jets,
        verbose = False,
        phi_mode = "COS2PHI" if phimodulation else "UNIFORM",
    ))

    setattr(process, name+'Table', SimonJetTableProducer.clone(
        src = name,
        name = name,
        verbose=False,
        isGen = False
    ))

    setattr(process, name+'Task', cms.Task(
        getattr(process, name),
        getattr(process, name+'Table'),
    ))
    process.schedule.associate(getattr(process, name+'Task'))

    return process

def setupFakeJets(process, 
                   jets, 
                   genjets,
                   name,
                   isMC,
                   genOnly,
                   phimodulation):
    if isMC:
        process = setupGenFakeShowerJets(process,
                                   genjets,
                                   name,
                                   phimodulation)
    if not genOnly:
        process = setupRecoFakeShowerJets(process,
                                    jets,
                                    name,
                                    ak8,
                                    phimodulation)
    return process



