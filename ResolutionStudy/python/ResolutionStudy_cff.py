import FWCore.ParameterSet.Config as cms

from SRothman.Matching.Matching_cff import *
from SRothman.CustomJets.CustomJets_cff import *
from SRothman.Matching.RuleMatchProducer_cfi import *
from SRothman.ResolutionStudy.ResolutionStudyTable_cfi import *

def setupResolutionStudy(process, verbose=0):
    process = addCustomJets(process, verbose=verbose, table=False)
    process.SimonJets.doEventSelection = False
    process.GenSimonJets.doEventSelection = False
    process.FullEventJets.doEventSelection = False
    process.GenFullEventJets.doEventSelection = False
    process.ShadowJets.doEventSelection = False
    process.GenShadowJets.doEventSelection = False

    for cutoff in [1, 2, 3, 4, 5, 6, 7, 10, 15, 20]:
        for pid in ['EM0', 'HAD0', 'HADCH', 'ELE', 'MU']:
            cutoffs = [1, 1, 1, 1, 1]
            if pid == 'EM0':
                cutoffs[0] = cutoff*cutoff
            elif pid == 'HAD0':
                cutoffs[1] = cutoff*cutoff
            elif pid == 'HADCH':
                cutoffs[2] = cutoff*cutoff
            elif pid == 'ELE':
                cutoffs[3] = cutoff*cutoff
            elif pid == 'MU':
                cutoffs[4] = cutoff*cutoff

            process = addGenMatching(process, verbose=verbose,
                                     name='%sCut%d' % (pid, cutoff),

                                     cutoffs = cutoffs,
                                     filters = ['AnyNeutral', 'AnyNeutralHadron', 
                                                'AnyChargedHadron', 
                                                'AnyElectron', 
                                                'AnyMuon'])

            process = addGenMatching(process, verbose=verbose,
                                     name='%sCut%dLimited' % (pid, cutoff),

                                     cutoffs = cutoffs,
                                     #these are EM0, HAD0, HADCH, ELE, MU
                                     filters = ['AnyPhoton', 'AnyNeutralHadron', 
                                                'AnyChargedHadron', 
                                                'AnyElectron', 
                                                'AnyMuon'])

    return process
