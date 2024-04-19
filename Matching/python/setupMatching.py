import FWCore.ParameterSet.Config as cms
from SRothman.Matching.GenMatchProducer_cfi import *
from SRothman.Matching.GenMatchTableProducer_cfi import *
from SRothman.CustomJets.SimonJetTableProducer_cfi import *

def setupMatching(process, verbose=0,
                  ak8=True,
                  name='GenMatch',
                  reco = 'SimonJets',
                  gen = 'GenSimonJets',
                  naive=False):
                   
    if naive:
        setattr(process, name, 
            NaiveMatchProducer.clone(
                reco = reco,
                gen = gen,
                verbose = verbose,
                jetMatchingDR = 0.4 if ak8 else 0.2
            )
        )
    else:
        setattr(process, name, 
            GenMatchProducer.clone(
                reco = reco,
                gen = gen,
                verbose = verbose,
                jetMatchingDR = 0.4 if ak8 else 0.2
            )
        )

    setattr(process, name+"Table", 
        GenMatchTableProducer.clone(
            src = name,
            name = name,
            verbose = verbose
        )
    )

    setattr(process, name+"ParticleTable", 
        SimonJetTableProducer.clone(
            src = reco,
            name = name+"Particles",
            verbose = verbose,
            isGen = False,
            addMatch = True,
            matchSrc = name,
            genJets = gen
        )
    )

    setattr(process, name+"GenParticleTable",
        SimonJetTableProducer.clone(
            src = gen,
            name = name+"GenParticles",
            verbose = verbose,
            isGen = True,
            addMatch = True,
            matchSrc = name,
        )
    )

    setattr(process, name+"Task", cms.Task(
        getattr(process, name),
        getattr(process, name+"Table"),
        getattr(process, name+"ParticleTable"),
        getattr(process, name+"GenParticleTable")
    ))
    process.schedule.associate(getattr(process, name+"Task"))

    return process
