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

    import itertools
    import numpy as np
    chmodes = ['Tight', 'Loose']
    sharethresholds = [0, 1, 2, 3, 5, 9999999999]
    em0recoverythresholds = [0, 1, 2, 3, 5, 9999999999]
    had0recoverythresholds = [0, 1, 2, 3, 5, 9999999999]
    for setting in itertools.product(chmodes, sharethresholds,
                                     em0recoverythresholds,
                                     had0recoverythresholds):

        chmode, sharethresh, em0thresh, had0thresh = setting
        name = '%sShare%dEM0%dHAD0%d'%(chmode, sharethresh,
                                       em0thresh, had0thresh)

        process = addGenMatching(process, verbose=verbose,
            name = name,

            ECALgranularityEta = [0.05, 0.05, 0.07],
            ECALgranularityPhi = [0.05, 0.05, 0.07],

            HCALgranularityEta = [0.10, 0.10, 0.15],
            HCALgranularityPhi = [0.10, 0.10, 0.15],

            CHangularEta = [0.010, 0.010, 0.015],
            CHangularPhi = [0.010, 0.010, 0.015],

            EM0thresholds =   [0.0, 0.0, 0.0],
            HAD0thresholds =  [0.0, 0.0, 0.0],
            HADCHthresholds = [0.0, 0.0, 0.0],
            ELEthresholds =   [0.0, 0.0, 0.0],
            MUthresholds =    [0.0, 0.0, 0.0],

            EM0dRcuts =   [0.050, 0.050, 0.050],
            HAD0dRcuts =  [0.100, 0.100, 0.100],
            HADCHdRcuts = [0.010, 0.010, 0.010],
            ELEdRcuts =   [0.005, 0.005, 0.005],
            MUdRcuts =    [0.005, 0.005, 0.005],

            hardflavorfilters = [
                'AnyNeutral',
                'AnyNeutralHadron',
                'AnyCharged' if chmode is 'Loose' else 'AnyChargedHadron',
                'AnyCharged' if chmode is 'Loose' else 'AnyElectron',
                'AnyCharged' if chmode is 'Loose' else 'AnyMuon'
            ],
            softflavorfilters = [
                'AnyPhoton',
                'AnyNeutralHadron',
                'AnyCharged' if chmode is 'Loose' else 'AnyChargedHadron',
                'AnyCharged' if chmode is 'Loose' else 'AnyElectron',
                'AnyCharged' if chmode is 'Loose' else 'AnyMuon'
            ],
            filterthresholds = [sharethresh, 0.0, 0.0, 0.0, 0.0],

            chargefilters = ['ChargeSign']*5,

            dropGenFilter = 'NONE',

            recoverLostTracks = em0thresh < 99999 or had0thresh < 99999,
            minRecoverPts = [em0thresh, had0thresh])
                
    return process
