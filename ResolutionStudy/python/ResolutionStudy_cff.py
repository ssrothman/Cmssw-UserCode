import FWCore.ParameterSet.Config as cms

from SRothman.Matching.Matching_cff import *
from SRothman.CustomJets.CustomJets_cff import *
from SRothman.Matching.RuleMatchProducer_cfi import *
from SRothman.ResolutionStudy.ResolutionStudyTable_cfi import *

def setupResolutionStudy(process, verbose=0):
    process = addCustomJets(process, verbose=verbose, table=False)
    process.SimonJets.doEventSelection = False
    process.GenSimonJets.doEventSelection = False

    process = addGenMatching(process, verbose=verbose,
                             name='DefaultMatch')
    process = addGenMatching(process, verbose=verbose,
                             name='NaiveMatch',
                             prefitters = ["Best","Best","Best"],
                             recoverLostTracks = False)
    process = addGenMatching(process, verbose=verbose,
                             name='NoDropMatch',
                             dropGenFilter = 'NONE',
                             dropRecoFilter = 'NONE')
    process = addGenMatching(process, verbose=verbose,
                             name='NoRecoverMatch',
                             recoverLostTracks = False)
    process = addGenMatching(process, verbose=verbose,
                             name='GlobalRefineMatch',
                             refiner='OneGenOneReco')
    process = addGenMatching(process, verbose=verbose,
                             name='NoRefineMatch',
                             refiner = 'None')
    return process
