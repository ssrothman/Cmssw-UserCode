import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.setupZMuMu import setupZMuMu
from SRothman.Analysis.setupRoccoR import setupRoccoR
from SRothman.CustomJets.setupCustomJets import setupCustomJets
from SRothman.CustomJets.setupPuppiJets import setupPuppiJets
from SRothman.EECs.setupEECs import setupEECs
from SRothman.Matching.setupMatching import setupMatching
from SRothman.CustomJets.setupAK8Jets import setupAK8Jets

def setupAnalysis(process, addNaive=True, ak8=True, addCharged=True, verbose=0):
    process = setupZMuMu(process)
    process = setupRoccoR(process)

    process = setupAK8Jets(process)
    process = setupPuppiJets(process)
    process = setupCustomJets(process, ak8=ak8)

    process = setupMatching(process, name='GenMatch',
                            reco='SimonJets',
                            gen='GenSimonJets')
    if addCharged:
        process = setupMatching(process, name='ChargedGenMatch',
                                reco='SimonChargedJets',
                                gen='GenSimonChargedJets')

    process = setupEECs(process, name='EECs', 
                        genMatch='GenMatch',
                        genjets='GenSimonJets', 
                        recojets='SimonJets',
                        verbose=verbose)
    if addCharged:
        process = setupEECs(process, name='ChargedEECs', 
                            genMatch='ChargedGenMatch',
                            genjets='GenSimonChargedJets', 
                            recojets='SimonChargedJets')
    if addNaive:
        process = setupMatching(process, name='NaiveGenMatch',
                                reco = 'SimonJets',
                                gen = 'GenSimonJets',
                                naive=True)
        if addCharged:
            process = setupMatching(process, name='NaiveChargedGenMatch',
                                    reco = 'SimonChargedJets',
                                    gen = 'GenSimonChargedJets',
                                    naive=True)

        process = setupEECs(process, name='NaiveEECs',
                            genMatch='NaiveGenMatch',
                            genjets = 'GenSimonJets',
                            recojets = 'SimonJets')
        if addCharged:
            process = setupEECs(process, name='NaiveChargedEECs', 
                                genMatch='NaiveChargedGenMatch',
                                genjets='GenSimonChargedJets', 
                                recojets='SimonChargedJets')
    return process
