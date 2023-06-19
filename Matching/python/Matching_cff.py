import FWCore.ParameterSet.Config as cms
from SRothman.Matching.GenMatchProducer_cfi import *
from SRothman.Matching.GenMatchTableProducer_cfi import *

def addGenMatching(process, verbose=0):
    process.GenMatch = GenMatchProducer.clone(
        reco = "SimonJets",
        gen = "GenSimonJets",
        recoParts = cms.InputTag("puppi"),
        genParts = cms.InputTag("packedGenParticlesForJetsNoNu"),
        doLargerCollections = cms.bool(False),
        verbose = verbose
    )
    process.GenMatchTable = GenMatchTableProducer.clone(
        src = "GenMatch",
        name = "GenMatch",
        verbose = verbose
    )
    process.MatchTask = cms.Task(process.GenMatch)
    process.MatchTableTask = cms.Task(process.GenMatchTable)
    process.schedule.associate(process.MatchTask)
    process.schedule.associate(process.MatchTableTask)

    process.SimonJetTable.addMatch = True
    process.SimonJetTable.isGen = False
    process.SimonJetTable.matchSrc = "GenMatch"
    #process.SimonJetTable.otherMatchSrc = "GenMatch:bigGen"
    #process.SimonJetTable.doOtherMatch = True

    process.GenSimonJetTable.addMatch = True
    process.GenSimonJetTable.isGen = True
    process.GenSimonJetTable.matchSrc = "GenMatch"
    #process.GenSimonJetTable.otherMatchSrc = "GenMatch:bigReco"
    #process.GenSimonJetTable.doOtherMatch = True

    return process
