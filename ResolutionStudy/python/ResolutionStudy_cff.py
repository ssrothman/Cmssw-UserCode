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

    for cutoff in [1, 2, 3, 5, 10]:
        process = addGenMatching(process, verbose=verbose,
                                name='HAD0Cut%d' % (cutoff),

                                cutoffs = [1, cutoff*cutoff, 1, 1, 1],
                                
                                ECALgranularityEta = [0.03, 0.03, 0.04],
                                ECALgranularityPhi = [0.03, 0.03, 0.04],

                                HCALgranularityEta = [0.05, 0.07, 0.10],
                                HCALgranularityPhi = [0.05, 0.07, 0.10],

                                CHangularEta = [0.01, 0.01, 0.01],
                                CHangularPhi = [0.01, 0.01, 0.01],

                                filters = ['AnyNeutral', 'AnyNeutralHadron', 
                                           'AnyChargedHadron', 
                                           'AnyElectron', 
                                           'AnyMuon'])

        process = addGenMatching(process, verbose=verbose,
                                name='ELEtightCut%d' % (cutoff),

                                cutoffs = [1, 1, 1, cutoff*cutoff, 1],
                                
                                ECALgranularityEta = [0.03, 0.03, 0.04],
                                ECALgranularityPhi = [0.03, 0.03, 0.04],

                                HCALgranularityEta = [0.05, 0.07, 0.10],
                                HCALgranularityPhi = [0.05, 0.07, 0.10],

                                CHangularEta = [0.01, 0.01, 0.01],
                                CHangularPhi = [0.01, 0.01, 0.01],

                                filters = ['AnyNeutral', 'AnyNeutralHadron', 
                                           'AnyChargedHadron', 
                                           'AnyElectron', 
                                           'AnyMuon'])

        process = addGenMatching(process, verbose=verbose,
                                name='ELEhadCut%d' % (cutoff),

                                cutoffs = [1, 1, 1, cutoff*cutoff, 1],
                                
                                ECALgranularityEta = [0.03, 0.03, 0.04],
                                ECALgranularityPhi = [0.03, 0.03, 0.04],

                                HCALgranularityEta = [0.05, 0.07, 0.10],
                                HCALgranularityPhi = [0.05, 0.07, 0.10],

                                CHangularEta = [0.01, 0.01, 0.01],
                                CHangularPhi = [0.01, 0.01, 0.01],

                                filters = ['AnyNeutral', 'AnyNeutralHadron', 
                                           'AnyChargedHadron', 
                                           'AnyChargedHadron', 
                                           'AnyMuon'])

        process = addGenMatching(process, verbose=verbose,
                                name='ELElooseCut%d' % (cutoff),

                                cutoffs = [1, 1, 1, cutoff*cutoff, 1],
                                
                                ECALgranularityEta = [0.03, 0.03, 0.04],
                                ECALgranularityPhi = [0.03, 0.03, 0.04],

                                HCALgranularityEta = [0.05, 0.07, 0.10],
                                HCALgranularityPhi = [0.05, 0.07, 0.10],

                                CHangularEta = [0.01, 0.01, 0.01],
                                CHangularPhi = [0.01, 0.01, 0.01],

                                filters = ['AnyNeutral', 'AnyNeutralHadron', 
                                           'AnyChargedHadron', 
                                           'AnyCharged', 
                                           'AnyMuon'])
    return process
