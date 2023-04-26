import FWCore.ParameterSet.Config as cms
from SRothman.Matching.GenMatchProducer_cfi import *
from SRothman.Matching.GenMatchTableProducer_cfi import *

def addGenMatching(process):
    process.GenMatch = GenMatchProducer.clone(
        reco = "SimonJets",
        gen = "GenSimonJets",
    )
    process.GenMatchTable = GenMatchTableProducer.clone(
        src = "GenMatch",
        name = "GenMatch"
    )
    process.MatchTask = cms.Task(process.GenMatch)
    process.MatchTableTask = cms.Task(process.GenMatchTable)
    process.schedule.associate(process.MatchTask)
    process.schedule.associate(process.MatchTableTask)

    process.SimonJetTable.addMatch = True
    process.SimonJetTable.isGen = False
    process.SimonJetTable.matchSrc = "GenMatch"

    process.GenSimonJetTable.addMatch = True
    process.GenSimonJetTable.isGen = True
    process.GenSimonJetTable.matchSrc = "GenMatch"

    return process
