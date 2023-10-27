import FWCore.ParameterSet.Config as cms

from SRothman.Matching.Matching_cff import *
from SRothman.CustomJets.CustomJets_cff import *
from SRothman.Matching.RuleMatchProducer_cfi import *
from SRothman.ResolutionStudy.ResolutionStudyTable_cfi import *


def setupPuppiTest(process):
    process = addCustomJets(process, verbose=1, table=True)
    process.SimonJets.doEventSelection = False
    process.GenSimonJets.doEventSelection = False
    process.FullEventJets.doEventSelection = False
    process.GenFullEventJets.doEventSelection = False
    process.ShadowJets.doEventSelection = False
    process.GenShadowJets.doEventSelection = False

    process.PFFullEventJets = process.FullEventJets.clone(
        partSrc=cms.InputTag("particleFlow"),
        maxNumPart = 4096,
    )
    process.PFFullEventJetTable = process.FullEventJetTable.clone(
        src = 'PFFullEventJets',
        name = 'PFFullEventJets',
    )
    process.SimonPFTask = cms.Task(process.PFFullEventJets,
                                   process.PFFullEventJetTable)
    process.schedule.associate(process.SimonPFTask)

    return process
