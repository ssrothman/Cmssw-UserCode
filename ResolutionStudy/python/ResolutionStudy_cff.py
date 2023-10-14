import FWCore.ParameterSet.Config as cms

from SRothman.Matching.Matching_cff import *
from SRothman.CustomJets.CustomJets_cff import *
from SRothman.Matching.RuleMatchProducer_cfi import *
from SRothman.ResolutionStudy.ResolutionStudyTable_cfi import *

pidDict = {'EM0': 0, 'HAD0': 1, 'HADCH': 2, 'ELE': 3, 'MU': 4}
elefilterdict = {'AnyElectron' : 'TightEle', 
              'AnyChargedHadron' : 'HadEle',
              'AnyChargedNoMu' : 'LooseNoMuEle',
              'AnyCharged' : 'LooseEle'}
phofilterdict = {'AnyPhoton' : 'TightPho',
                 'AnyNeutral' : 'LoosePho'}
dropgendict = {'NONE' : 'NoDrop',
               'ALL' : 'Drop'}
recoverdict = {False : 'Norecover',
               True : 'Recover'}

def setupResolutionStudy(process, verbose=0):
    process = addCustomJets(process, verbose=verbose, table=False)
    process.SimonJets.doEventSelection = False
    process.GenSimonJets.doEventSelection = False
    process.FullEventJets.doEventSelection = False
    process.GenFullEventJets.doEventSelection = False
    process.ShadowJets.doEventSelection = False
    process.GenShadowJets.doEventSelection = False

    hadchfilters = ['Tight', 'Loose']
    had0filters = ['Tight', 'Loose']
    elefilters = ['Tight', 'Loose']
    em0filters = ['Tight', 'Loose']

    dropgens = ['Yes', 'No']
    chargefilters = ['Any', 'Tight']
    recover = ["Free", 'Limited', 'No']
    thresholds = ['Yes', 'No']

    import itertools

    for setting in itertools.product(hadchfilters, had0filters,
                                     elefilters, em0filters,
                                     dropgens, chargefilters, 
                                     recover, thresholds):
        print(setting)
        hadchfilter, had0filter, elefilter, em0filter,\
            dropgen, chargefilter, recover, threshold = setting

        name = '%s%s%s%s%s%s%s%s'%(hadchfilter, had0filter, elefilter, em0filter,
                                   dropgen, chargefilter, recover, threshold)

        process = addGenMatching(process, verbose=verbose,
                             name = name,

                             ECALgranularityEta = [0.05, 0.05, 0.07],
                             ECALgranularityPhi = [0.05, 0.05, 0.07],

                             HCALgranularityEta = [0.10, 0.10, 0.15],
                             HCALgranularityPhi = [0.10, 0.10, 0.15],

                             CHangularEta = [0.010, 0.010, 0.015],
                             CHangularPhi = [0.010, 0.010, 0.015],

                             EM0thresholds =   [0.60, 0.30, 0.30] if threshold == 'Yes' else [0.0, 0.0, 0.0],
                             HAD0thresholds =  [0.70, 0.40, 0.40] if threshold == 'Yes' else [0.0, 0.0, 0.0],
                             HADCHthresholds = [0.15, 0.17, 0.10] if threshold == 'Yes' else [0.0, 0.0, 0.0],
                             ELEthresholds =   [2.20, 2.50, 3.50] if threshold == 'Yes' else [0.0, 0.0, 0.0],
                             MUthresholds =    [3.00, 1.10, 0.80] if threshold == 'Yes' else [0.0, 0.0, 0.0],

                             EM0dRcuts = [0.05, 0.05, 0.07],
                             HAD0dRcuts = [0.10, 0.10, 0.15],
                             HADCHdRcuts = [0.010, 0.010, 0.015],
                             ELEdRcuts = [0.003, 0.003, 0.005],
                             MUdRcuts = [0.003, 0.003, 0.005],

                             flavorfilters = ['AnyPhoton' if em0filter == 'Tight' else 'AnyNeutral',
                                              'AnyNeutralHadron' if had0filter == 'Tight' else 'AnyNeutral',
                                              'AnyChargedHadron' if hadchfilter == 'Tight' else 'AnyChargedNoMu',
                                              'AnyElectron' if elefilter == 'Tight' else 'AnyChargedNoMu',
                                              'AnyMuon'],

                             chargefilters = ['ChargeSign' if chargefilter == 'Tight' else 'Any'] * 5,

                             dropGenFilter = 'NONE' if dropgen == 'No' else 'ALL',

                             recoverLostTracks = False if recover == 'No' else True,
                             minRecoverPts = [0.0, 0.0] if recover == 'Free' else [3.0, 5.0])
                
    return process
