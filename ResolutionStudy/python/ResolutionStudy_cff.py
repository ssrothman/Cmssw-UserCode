import FWCore.ParameterSet.Config as cms

from SRothman.CustomJets.CustomJets_cff import *
from SRothman.Matching.RuleMatchProducer_cfi import *
from SRothman.Matching.GenMatchProducer_cfi import *
from SRothman.ResolutionStudy.ResolutionStudyTable_cfi import *

def setupResolutionStudy(process, verbose=0, useRuleMatch=True):
    process = addCustomJets(process, 0)

    process.RuleMatchProducer = RuleMatchProducer.clone(
        verbose = verbose,
        reco = 'SimonJets',
        gen = 'GenSimonJets'
    )
    process.GenMatchProducer = GenMatchProducer.clone(
        verbose = verbose,
        reco = 'SimonJets',
        gen = 'GenSimonJets'
    )
    process.ResolutionStudyTable = ResolutionStudyTable.clone(
        verbose = verbose,
        src = 'RuleMatchProducer' if useRuleMatch else 'GenMatchProducer'
    )
    process.SimonJets.doEventSelection = False
    process.GenSimonJets.doEventSelection = False
    if useRuleMatch:
        process.ResolutionStudyTask = cms.Task(process.RuleMatchProducer, process.ResolutionStudyTable)
    else:
        process.ResolutionStudyTask = cms.Task(process.GenMatchProducer, process.ResolutionStudyTable)
    process.schedule.associate(process.ResolutionStudyTask)
    return process
