import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.SelectZMuMu_cff import *
from SRothman.Analysis.addRoccoR_cff import *
from SRothman.CustomJets.CustomJets_cff import *
from SRothman.Matching.Matching_cff import *
from SRothman.EECs.EECs_cff import *

def setupEECtest(process, verbose):
    process = addCustomJets(process, verbose=0, table=True)
    process.SimonJets.doEventSelection = False
    process.GenSimonJets.doEventSelection = False
    process.FullEventJets.doEventSelection = False
    process.GenFullEventJets.doEventSelection = False
    process.ShadowJets.doEventSelection = False
    process.GenShadowJets.doEventSelection = False

    process = addGenMatching(process, verbose=0,
            name = 'CaloShare',

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
            ELEdRcuts =   [0.005, 0.005, 0.007],
            MUdRcuts =    [0.005, 0.005, 0.007],

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
                'AnyCharged',
            ],
            filterthresholds = [3.0, 0.0, 0.0, 0.0, 0.0],

            chargefilters = ['ChargeSign']*5,

            dropGenFilter = 'NONE',

            recoverLostTracks = True,
            HADCHrecoverThresholds = [5.0, 5.0, 5.0],
            ELErecoverThresholds = [5.0, 5.0, 5.0],

            recoverLostHAD0 = False,
            HAD0recoverThresholds = [5.0, 5.0, 5.0])

    process = addGenMatching(process, verbose=0,
            name = 'CaloRecover',

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
            ELEdRcuts =   [0.005, 0.005, 0.007],
            MUdRcuts =    [0.005, 0.005, 0.007],

            hardflavorfilters = [
                'AnyPhoton',
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
                'AnyCharged',
            ],
            filterthresholds = [3.0, 0.0, 0.0, 0.0, 0.0],

            chargefilters = ['ChargeSign']*5,

            dropGenFilter = 'NONE',

            recoverLostTracks = True,
            HADCHrecoverThresholds = [5.0, 5.0, 5.0],
            ELErecoverThresholds = [5.0, 5.0, 5.0],

            recoverLostHAD0 = True,
            HAD0recoverThresholds = [5.0, 5.0, 5.0])

    process = addGenMatching(process, verbose=0,
            name = 'NaiveMatch',

            ECALgranularityEta = [0.05, 0.05, 0.05],
            ECALgranularityPhi = [0.05, 0.05, 0.05],

            HCALgranularityEta = [0.05, 0.05, 0.05],
            HCALgranularityPhi = [0.05, 0.05, 0.05],

            CHangularEta = [0.050, 0.050, 0.050],
            CHangularPhi = [0.050, 0.050, 0.050],

            EM0thresholds =   [0.0, 0.0, 0.0],
            HAD0thresholds =  [0.0, 0.0, 0.0],
            HADCHthresholds = [0.0, 0.0, 0.0],
            ELEthresholds =   [0.0, 0.0, 0.0],
            MUthresholds =    [0.0, 0.0, 0.0],

            EM0dRcuts =   [0.050, 0.050, 0.050],
            HAD0dRcuts =  [0.050, 0.050, 0.050],
            HADCHdRcuts = [0.050, 0.050, 0.050],
            ELEdRcuts =   [0.050, 0.050, 0.050],
            MUdRcuts =    [0.050, 0.050, 0.050],

            refiner = 'OneGenOneReco',
            prefitters = ['Best']*5,
                                    
            hardflavorfilters = [
                'AnyNeutral',
                'AnyNeutral',
                'AnyCharged',
                'AnyCharged',
                'AnyCharged'
            ],
            softflavorfilters = [
                'AnyNeutral',
                'AnyNeutral',
                'AnyCharged',
                'AnyCharged',
                'AnyCharged',
            ],
            filterthresholds = [0.0, 0.0, 0.0, 0.0, 0.0],

            chargefilters = ['ChargeSign']*5,

            dropGenFilter = 'NONE',

            recoverLostTracks = False,
            recoverLostHAD0 = False)

    process = addEECs(process, verbose,
                      name = 'CaloShareEEC',
                      genMatch = 'CaloShare',
                      normToRaw=True)

    process = addEECs(process, verbose,
                      name = 'CaloRecoverEEC',
                      genMatch = 'CaloRecover',
                      normToRaw=True)

    process = addEECs(process, verbose,
                      name = 'CaloShareCorrEEC',
                      genMatch = 'CaloShare',
                      normToRaw=False)

    process = addEECs(process, verbose,
                      name = 'CaloShareCorrEEC',
                      genMatch = 'CaloShare',
                      normToRaw=False)

    process = addEECs(process, verbose,
                      name = 'NaiveEEC',
                      genMatch = 'NaiveMatch',
                      normToRaw=True)

    process = addEECs(process, verbose,
                      name = 'NaiveEEC',
                      genMatch = 'NaiveMatch',
                      normToRaw=False)
    return process
 
