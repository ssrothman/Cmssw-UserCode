import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.setupZMuMu import setupZMuMu
from SRothman.Analysis.setupRoccoR import setupRoccoR
from SRothman.CustomJets.setupCustomJets import setupCustomJets
from SRothman.CustomJets.setupPuppiJets import setupPuppiJets
from SRothman.EECs.setupEECs import setupEECs
from SRothman.Matching.setupMatching import setupMatching
from SRothman.CustomJets.setupAK8Jets import setupAK8Jets
from SRothman.CustomJets.setupCHSJets import setupCHSJets

def setupAnalysis(process, doNominal=True, addNaive=True, 
                  ak8=True, doCharged=True, addFullEvent=True,
                  addRandomControl=True, addZControl=True,
                  verbose=0, isMC=True):
    process = setupZMuMu(process)
    process = setupRoccoR(process, isMC=isMC)

    process = setupCHSJets(process)
    process = setupAK8Jets(process, isMC=isMC)
    process = setupPuppiJets(process, isMC=isMC)
    process = setupCustomJets(process, ak8=ak8, isMC=isMC)

    names = []
    matchnames = []
    EECnames = []

    if doNominal:
        names += ['SimonJets']
        matchnames += ['GenMatch']
        EECnames += ['EECs']
    if doCharged:
        names += ['ChargedSimonJets']
        matchnames += ['ChargedGenMatch']
        EECnames += ['ChargedEECs']

    if addFullEvent:
        if doNominal:
            names += ['FullEventJets']
            matchnames += ['FullEventGenMatch']
            EECnames += ['FullEventEECs']
        if doCharged:
            names += ['ChargedFullEventJets']
            matchnames += ['ChargedFullEventGenMatch']
            EECnames += ['ChargedFullEventEECs']

    if addRandomControl:
        if doNominal:
            names += ['RandomConeJets']
            matchnames += ['RandomConeGenMatch']
            EECnames += ['RandomConeEECs']
        if doCharged:
            names += ['ChargedRandomConeJets']
            matchnames += ['ChargedRandomConeGenMatch']
            EECnames += ['ChargedRandomConeEECs']

    if addZControl:
        if doNominal:
            names += ['ControlConeJets']
            matchnames += ['ControlConeGenMatch']
            EECnames += ['ControlConeEECs']
        if doCharged:
            names += ['ChargedControlConeJets']
            matchnames += ['ChargdControlConeGenMatch']
            EECnames += ['ChargedControlConeEECs']

    if addNaive:
        extranames = names[:]
        names += extranames
        extramatchnames = ['Naive'+name for name in matchnames]
        matchnames += extramatchnames
        extraEECnames = ['Naive'+name for name in EECnames]
        EECnames += extraEECnames

    print("RUNNING")

    for name, matchname, EECname in zip(names, matchnames, EECnames):
        print(name, matchname, EECname)
        rungen = isMC and not ('RandomCone' in name or 'ControlCone' in name)
        if rungen:
            process = setupMatching(process, name=matchname,
                                    reco=name,
                                    gen='Gen'+name,
                                    naive = 'Naive' in matchname,
                                    ak8 = ak8,
                                    verbose=verbose)

        process = setupEECs(process, name=EECname,
                            genMatch = matchname,
                            genjets = 'Gen' + name,
                            recojets = name,
                            verbose=verbose, isMC=rungen)

    print()

    return process
