import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.setupZMuMu import setupZMuMu
from SRothman.Analysis.setupRoccoR import setupRoccoR
from SRothman.CustomJets.setupCustomJets import setupCustomJets
from SRothman.EECs.setupEECs import setupEECs
from SRothman.Matching.setupMatching import setupMatching

def setupAnalysis(process):
    process = setupZMuMu(process)
    process = setupRoccoR(process)
    process = setupCustomJets(process)
    process = setupMatching(process, name='GenMatch',
                            reco='SimonJets',
                            gen='GenSimonJets')
    process = setupMatching(process, name='ChargedGenMatch',
                            reco='SimonChargedJets',
                            gen='GenSimonChargedJets')
    process = setupEECs(process, name='EECs', 
                        genMatch='GenMatch',
                        genjets='GenSimonJets', 
                        recojets='SimonJets')
    process = setupEECs(process, name='ChargedEECs', 
                        genMatch='ChargedGenMatch',
                        genjets='GenSimonChargedJets', 
                        recojets='SimonChargedJets')
    return process
