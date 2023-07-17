import FWCore.ParameterSet.Config as cms
from SRothman.Matching.GenMatchProducer_cfi import *
from SRothman.Matching.GenMatchTableProducer_cfi import *
from SRothman.CustomJets.SimonJetTableProducer_cfi import *

def addGenMatching(process, verbose=0,
                  name='GenMatch',
                  filter = GenMatchProducer.filter,
                  uncertainty = GenMatchProducer.uncertainty,
                  prefitters = GenMatchProducer.prefitters,
                  recoverLostTracks = GenMatchProducer.recoverLostTracks,
                  cutoff = GenMatchProducer.cutoff):

    setattr(process, name, 
        GenMatchProducer.clone(
            reco = "SimonJets",
            gen = "GenSimonJets",
            recoParts = cms.InputTag("puppi"),
            genParts = cms.InputTag("packedGenParticlesForJetsNoNu"),
            doLargerCollections = cms.bool(False),
            verbose = verbose,

            filter = filter,
            uncertainty = uncertainty,
            prefitters = prefitters,
            recoverLostTracks = recoverLostTracks,
            cutoff = cutoff
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
            src = 'SimonJets',
            name = name+"Particles",
            verbose = verbose,
            isGen = False,
            addMatch = True,
            matchSrc = name,
            genJets = 'GenSimonJets'
        )
    )

    setattr(process, name+"GenParticleTable",
        SimonJetTableProducer.clone(
            src = 'GenSimonJets',
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
