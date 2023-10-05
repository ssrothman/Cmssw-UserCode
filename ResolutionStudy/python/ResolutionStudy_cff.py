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

    for cutoff in [2, 4, 8, 16, 32, 64]:
        process = addGenMatching(process, verbose=verbose,
                                 name='Cut%d' % cutoff,

                                 cutoffs = [cutoff] * 5,
                                 filters = ['AnyNeutral', 'AnyNeutralHadron', 
                                            'AnyChargedHadron', 
                                            'AnyElectron', 
                                            'AnyMuon'])

        process = addGenMatching(process, verbose=verbose,
                                 name='Cut%dLimited' % cutoff,

                                 cutoffs = [cutoff] * 5,
                                 #these are EM0, HAD0, HADCH, ELE, MU
                                 filters = ['AnyPhoton', 'AnyNeutralHadron', 
                                            'AnyChargedHadron', 
                                            'AnyElectron', 
                                            'AnyMuon'])

    return process
