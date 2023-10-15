import FWCore.ParameterSet.Config as cms

from SRothman.Matching.Matching_cff import *
from SRothman.CustomJets.CustomJets_cff import *
from SRothman.Matching.RuleMatchProducer_cfi import *
from SRothman.ResolutionStudy.ResolutionStudyTable_cfi import *

chargedfilters = ['Tight', 'Loose', 'Looser']
calosharings = ['Off', 'Normal', 'Thresholded', 'GenThresholded', 'Both']
recoveries = ['Free', 'Limited', 'No']

def setupResolutionStudy(process, verbose=0):
    process = addCustomJets(process, verbose=verbose, table=False)
    process.SimonJets.doEventSelection = False
    process.GenSimonJets.doEventSelection = False
    process.FullEventJets.doEventSelection = False
    process.GenFullEventJets.doEventSelection = False
    process.ShadowJets.doEventSelection = False
    process.GenShadowJets.doEventSelection = False

    import itertools

    for setting in itertools.product(chargedfilters, calosharings, recoveries):
        print(setting)
        chargedfilter, calosharing, recover = setting

        name = '%s%s%s'%(chargedfilter, calosharing, recover)

        if chargedfilter == 'Loose':
            loosech = 'AnyChargedNoMu'
        elif chargedfilter == 'Looser':
            loosech = 'AnyCharged'

        hardflavorfilters = [
            'AnyNeutral',
            'AnyNeutralHadron',
            'AnyChargedHadron' if chargedfilter == 'Tight' else loosech,
            'AnyElectron' if chargedfilter == 'Tight' else loosech,
            'AnyCharged' if chargedfilter == 'Looser' else 'AnyMuon'
        ]
        softflavorfilters = hardflavorfilters[:]
        filterthresholds = [0.0, 0.0, 0.0, 0.0, 0.0]

        if calosharing == 'Thresholded':
            hardflavorfilters[0] = 'AnyNeutral'
            softflavorfilters[0] = 'AnyPhoton'
            filterthresholds[0] = 1.0
        elif calosharing == 'GenThresholded':
            hardflavorfilters[0] = 'AnyPhotonHardHadron1'
            filterthresholds[0] = 0.0
        elif calosharing == 'Off':
            hardflavorfilters[0] = 'AnyPhoton'
            filterthresholds[0] = 0.0
        elif calosharing == 'Normal':
            hardflavorfilters[0] = 'AnyNeutral'
            filterthresholds[0] = 0.0
        elif calosharing == 'Both':
            hardflavorfilters[0] = 'AnyNeutral'
            hardflavorfilters[1] = 'AnyNeutral'
            filterthresholds[0] = 0.0

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

                             EM0dRcuts =   [0.050, 0.050, 0.070],
                             HAD0dRcuts =  [0.100, 0.100, 0.150],
                             HADCHdRcuts = [0.010, 0.010, 0.015],
                             ELEdRcuts =   [0.003, 0.003, 0.005],
                             MUdRcuts =    [0.003, 0.003, 0.005],

                             hardflavorfilters = hardflavorfilters,
                             softflavorfilters = softflavorfilters,
                             filterthresholds = filterthresholds,

                             chargefilters = ['ChargeSign']*5,

                             dropGenFilter = 'NONE',

                             recoverLostTracks = recover != 'No',
                             minRecoverPts = [0.2, 1.0] if recover == 'Limited' else [0.0, 0.0])
                
    return process
