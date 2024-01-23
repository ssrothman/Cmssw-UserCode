import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.SelectZMuMu_cff import *
from SRothman.Analysis.addRoccoR_cff import *
from SRothman.CustomJets.CustomJets_cff import *
from SRothman.Matching.Matching_cff import *
from SRothman.EECs.EECs_cff import *

def setupEECtest(process, verbose):
    process = setupCustomJets(process, verbose=0, table=True,
                              EM0threshold = -1.0,
                              HAD0threshold = -1.0,
                              HADCHthreshold = -1.0,
                              ELEthreshold = -1.0,
                              MUthreshold = -1.0,
                              onlyFromPV = True,
                              onlyCharged = True,

                              minPartPt_GEN = -1.0,
                              )

    process = addRoccoR(process, verbose=False)
    process = addZMuMuEventSelection(process, verbose=False)

    evtsel = False
    process.SimonJets.doEventSelection = evtsel
    process.GenSimonJets.doEventSelection = evtsel
    #process.FullEventJets.doEventSelection = evtsel
    #process.GenFullEventJets.doEventSelection = evtsel
    #process.ShadowJets.doEventSelection = evtsel
    #process.GenShadowJets.doEventSelection = evtsel

    process = addGenMatching(process, verbose=0,
            name = 'TrackDRLooseSlopeMatch',

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

            dRfilters = ['Fixed', 'Fixed', 
                         'Tracking', 'Tracking', 'Tracking'],

            EM0constDR =   [0.050, 0.050, 0.070],
            EM0floatDR =   [0.000, 0.000, 0.000],
            EM0capDR =     [0.000, 0.000, 0.000],

            HAD0constDR =  [0.100, 0.100, 0.150],
            HAD0floatDR =  [0.000, 0.000, 0.000],
            HAD0capDR =    [0.000, 0.000, 0.000],

            HADCHconstDR = [0.002, 0.002, 0.003],
            HADCHfloatDR = [0.015, 0.015, 0.020],
            HADCHcapDR =   [0.050, 0.050, 0.070],

            ELEconstDR =   [0.002, 0.002, 0.003],
            ELEfloatDR =   [0.015, 0.015, 0.020],
            ELEcapDR =     [0.050, 0.050, 0.070],

            MUconstDR =    [0.002, 0.002, 0.003],
            MUfloatDR =    [0.015, 0.015, 0.020],
            MUcapDR =      [0.050, 0.050, 0.070],

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
            propagateLostTracks = True,
            HADCHrecoverThresholds = [5.0, 5.0, 5.0],
            ELErecoverThresholds = [5.0, 5.0, 5.0],

            recoverLostHAD0 = False,
            HAD0recoverThresholds = [5.0, 5.0, 5.0])



    process = addGenMatching(process, verbose=0,
            name = 'TrackDRLooseConstMatch',

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

            dRfilters = ['Fixed', 'Fixed', 
                         'Tracking', 'Tracking', 'Tracking'],

            EM0constDR =   [0.050, 0.050, 0.070],
            EM0floatDR =   [0.000, 0.000, 0.000],
            EM0capDR =     [0.000, 0.000, 0.000],

            HAD0constDR =  [0.100, 0.100, 0.150],
            HAD0floatDR =  [0.000, 0.000, 0.000],
            HAD0capDR =    [0.000, 0.000, 0.000],

            HADCHconstDR = [0.005, 0.005, 0.007],
            HADCHfloatDR = [0.010, 0.010, 0.015],
            HADCHcapDR =   [0.050, 0.050, 0.070],

            ELEconstDR =   [0.005, 0.005, 0.007],
            ELEfloatDR =   [0.010, 0.010, 0.015],
            ELEcapDR =     [0.050, 0.050, 0.070],

            MUconstDR =    [0.005, 0.005, 0.007],
            MUfloatDR =    [0.010, 0.010, 0.015],
            MUcapDR =      [0.050, 0.050, 0.070],

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
            propagateLostTracks = True,
            HADCHrecoverThresholds = [5.0, 5.0, 5.0],
            ELErecoverThresholds = [5.0, 5.0, 5.0],

            recoverLostHAD0 = False,
            HAD0recoverThresholds = [5.0, 5.0, 5.0])



    process = addGenMatching(process, verbose=0,
            name = 'TrackDRMatch',

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

            dRfilters = ['Fixed', 'Fixed', 
                         'Tracking', 'Tracking', 'Tracking'],

            EM0constDR =   [0.050, 0.050, 0.070],
            EM0floatDR =   [0.000, 0.000, 0.000],
            EM0capDR =     [0.000, 0.000, 0.000],

            HAD0constDR =  [0.100, 0.100, 0.150],
            HAD0floatDR =  [0.000, 0.000, 0.000],
            HAD0capDR =    [0.000, 0.000, 0.000],

            HADCHconstDR = [0.002, 0.002, 0.003],
            HADCHfloatDR = [0.010, 0.010, 0.015],
            HADCHcapDR =   [0.050, 0.050, 0.070],

            ELEconstDR =   [0.002, 0.002, 0.003],
            ELEfloatDR =   [0.010, 0.010, 0.015],
            ELEcapDR =     [0.050, 0.050, 0.070],

            MUconstDR =    [0.002, 0.002, 0.003],
            MUfloatDR =    [0.010, 0.010, 0.015],
            MUcapDR =      [0.050, 0.050, 0.070],

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
            propagateLostTracks = True,
            HADCHrecoverThresholds = [5.0, 5.0, 5.0],
            ELErecoverThresholds = [5.0, 5.0, 5.0],

            recoverLostHAD0 = False,
            HAD0recoverThresholds = [5.0, 5.0, 5.0])

    process = addGenMatching(process, verbose=0,
            name = 'FixedDRLooseMatch',

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

            dRfilters = ['Fixed', 'Fixed', 
                         'Fixed', 'Fixed', 'Fixed'],

            EM0constDR =   [0.070, 0.070, 0.100],
            EM0floatDR =   [0.000, 0.000, 0.000],
            EM0capDR =     [0.000, 0.000, 0.000],

            HAD0constDR =  [0.150, 0.150, 0.200],
            HAD0floatDR =  [0.000, 0.000, 0.000],
            HAD0capDR =    [0.000, 0.000, 0.000],

            HADCHconstDR = [0.010, 0.010, 0.015],
            HADCHfloatDR = [0.010, 0.010, 0.010],
            HADCHcapDR =   [0.050, 0.050, 0.050],

            ELEconstDR =   [0.010, 0.010, 0.015],
            ELEfloatDR =   [0.000, 0.000, 0.000],
            ELEcapDR =     [0.050, 0.050, 0.070],

            MUconstDR =    [0.010, 0.010, 0.015],
            MUfloatDR =    [0.010, 0.010, 0.010],
            MUcapDR =      [0.050, 0.050, 0.050],

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
            propagateLostTracks = True,
            HADCHrecoverThresholds = [5.0, 5.0, 5.0],
            ELErecoverThresholds = [5.0, 5.0, 5.0],

            recoverLostHAD0 = False,
            HAD0recoverThresholds = [5.0, 5.0, 5.0])



    process = addGenMatching(process, verbose=0,
            name = 'FixedDRMatch',

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

            dRfilters = ['Fixed', 'Fixed', 
                         'Fixed', 'Fixed', 'Fixed'],

            EM0constDR =   [0.050, 0.050, 0.070],
            EM0floatDR =   [0.000, 0.000, 0.000],
            EM0capDR =     [0.000, 0.000, 0.000],

            HAD0constDR =  [0.100, 0.100, 0.150],
            HAD0floatDR =  [0.000, 0.000, 0.000],
            HAD0capDR =    [0.000, 0.000, 0.000],

            HADCHconstDR = [0.005, 0.005, 0.007],
            HADCHfloatDR = [0.010, 0.010, 0.010],
            HADCHcapDR =   [0.050, 0.050, 0.050],

            ELEconstDR =   [0.005, 0.005, 0.007],
            ELEfloatDR =   [0.000, 0.000, 0.000],
            ELEcapDR =     [0.050, 0.050, 0.070],

            MUconstDR =    [0.005, 0.005, 0.007],
            MUfloatDR =    [0.010, 0.010, 0.010],
            MUcapDR =      [0.050, 0.050, 0.050],

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
            propagateLostTracks = True,
            HADCHrecoverThresholds = [5.0, 5.0, 5.0],
            ELErecoverThresholds = [5.0, 5.0, 5.0],

            recoverLostHAD0 = False,
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

            dRfilters = ['Fixed', 'Fixed', 
                         'Fixed', 'Fixed', 'Fixed'],

            EM0constDR =   [0.050, 0.050, 0.050],
            HAD0constDR =  [0.050, 0.050, 0.050],
            HADCHconstDR = [0.050, 0.050, 0.050],
            ELEconstDR =   [0.050, 0.050, 0.050],
            MUconstDR =    [0.050, 0.050, 0.050],

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
            propagateLostTracks = True,
            recoverLostHAD0 = False)

    process = addEECs(process, verbose,
                      name = 'FixedDR',
                      genMatch = 'FixedDRMatch',
                      genjets = 'GenSimonJets',
                      recojets = 'SimonJets',
                      normToRaw=True)

    process = addEECs(process, verbose,
                      name = 'FixedDRLoose',
                      genMatch = 'FixedDRLooseMatch',
                      genjets = 'GenSimonJets',
                      recojets = 'SimonJets',
                      normToRaw=True)

    process = addEECs(process, verbose,
                      name = 'TrackDR',
                      genMatch = 'TrackDRMatch',
                      genjets = 'GenSimonJets',
                      recojets = 'SimonJets',
                      normToRaw=True)

    process = addEECs(process, verbose,
                      name = 'TrackDRLooseSlope',
                      genMatch = 'TrackDRLooseSlopeMatch',
                      genjets = 'GenSimonJets',
                      recojets = 'SimonJets',
                      normToRaw=True)

    process = addEECs(process, verbose,
                      name = 'TrackDRLooseConst',
                      genMatch = 'TrackDRLooseConstMatch',
                      genjets = 'GenSimonJets',
                      recojets = 'SimonJets',
                      normToRaw=True)

    process = addEECs(process, verbose,
                      name = 'Naive',
                      genMatch = 'NaiveMatch',
                      genjets = 'GenSimonJets',
                      recojets = 'SimonJets',
                      normToRaw=True)

    return process
 
