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

    multipliers = [0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8, 2.0]
    for mult in multipliers:
        name = 'dRx%d'%(mult*10)

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

                             EM0dRcuts =   [0.050*mult, 0.050*mult, 0.050*mult],
                             HAD0dRcuts =  [0.100*mult, 0.100*mult, 0.100*mult],
                             HADCHdRcuts = [0.010*mult, 0.010*mult, 0.010*mult],
                             ELEdRcuts =   [0.005*mult, 0.005*mult, 0.005*mult],
                             MUdRcuts =    [0.005*mult, 0.005*mult, 0.005*mult],

                             hardflavorfilters = [
                                'AnyNeutral',
                                'AnyNeutralHadron',
                                'AnyCharged',
                                'AnyCharged',
                                'AnyCharged'
                             ],
                             softflavorfilters = [
                                'AnyPhoton',
                                'AnyNeutralHadron',
                                'AnyCharged',
                                'AnyCharged',
                                'AnyCharged'
                             ],
                             filterthresholds = [4.0, 0.0, 0.0, 0.0, 0.0],

                             chargefilters = ['ChargeSign']*5,

                             dropGenFilter = 'NONE',

                             recoverLostTracks = True,
                             minRecoverPts = [2.0, 5.0])
                
    return process
