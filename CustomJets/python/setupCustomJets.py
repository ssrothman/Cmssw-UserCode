import FWCore.ParameterSet.Config as cms

from SRothman.CustomJets.setupPuppiJets import *
from SRothman.CustomJets.setupSimonJets import *
from SRothman.CustomJets.setupShadowJets import *

def setupCustomJets(process, verbose=False, table=False, 
                    inputJets = 'patJetsReapplyJECPuppi',
                    inputGenJets = 'slimmedGenJets',

                    jets = 'updatedJetsPuppi',
                    CHSjets = 'finalJets',
                    genJets = 'selectedGenJets',

                    parts = 'packedPFCandidates',
                    genParts = 'packedGenParticles'):

    process = setupPuppiJets(process, inputJets, inputGenJets)

    process = setupSimonJets(process, jets, genJets,
                             CHSjets, False, 'ZMuMu',
                             'SimonJets')
    process = setupSimonJets(process, jets, genJets,
                             CHSjets, True, 'ZMuMu',
                             'SimonChargedJets')
    return process
