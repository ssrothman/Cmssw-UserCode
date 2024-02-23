import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.setupZMuMu import setupZMuMu
from SRothman.Analysis.setupRoccoR import setupRoccoR
from SRothman.CustomJets.setupCustomJets import setupCustomJets
from SRothman.CustomJets.setupPuppiJets import setupPuppiJets
from SRothman.EECs.setupEECs import setupEECs
from SRothman.Matching.setupMatching import setupMatching
from SRothman.CustomJets.setupAK8Jets import setupAK8Jets

def setupAnalysis(process, doNominal=True, addNaive=True, 
                  ak8=True, addCharged=True, addFullEvent=True,
                  addRandomControl=True, addZControl=True,
                  verbose=0, isMC=True):
    process = setupZMuMu(process)
    process = setupRoccoR(process, isMC=isMC)

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
    if addFullEvent:
        names += ['FullEventJets']
        matchnames += ['FullEventGenMatch']
        EECnames += ['FullEventEECs']
    if addRandomControl:
        names += ['RandomConeJets']
        matchnames += ['RandomConeGenMatch']
        EECnames += ['RandomConeEECs']
    if addZControl:
        names += ['ControlConeJets']
        matchnames += ['ControlConeGenMatch']
        EECnames += ['ControlConeEECs']

    if addCharged:
        extranames = ['Charged'+name for name in names]
        names += extranames
        extramatchnames = ['Charged'+name for name in matchnames]
        matchnames += extramatchnames
        extraEECnames = ['Charged'+name for name in EECnames]
        EECnames += extraEECnames

    if addNaive:
        extranames = names[:]
        names += extranames
        extramatchnames = ['Naive'+name for name in matchnames]
        matchnames += extramatchnames
        extraEECnames = ['Naive'+name for name in EECnames]
        EECnames += extraEECnames

    for name, matchname, EECname in zip(names, matchnames, EECnames):
        if isMC:
            process = setupMatching(process, name=matchname,
                                    reco=name,
                                    gen='Gen'+name,
                                    naive = 'Naive' in matchname)

        process = setupEECs(process, name=EECname,
                            genMatch = matchname,
                            genjets = 'Gen' + name,
                            recojets = name,
                            verbose=verbose, isMC=isMC)

    return process
