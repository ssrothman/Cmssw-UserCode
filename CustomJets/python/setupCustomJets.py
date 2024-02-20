import FWCore.ParameterSet.Config as cms

from SRothman.CustomJets.setupPuppiJets import *
from SRothman.CustomJets.setupSimonJets import *
from SRothman.CustomJets.setupShadowJets import *

def setupCustomJets(process, verbose=False, table=False, ak8 = False):

    parts = 'packedPFCandidates'
    genParts = 'packedGenParticles'

    if ak8:
        jets = 'selectedUpdatedJetsAK8'
        genJets = 'selectedGenJetsAK8'
        CHSjets = 'finalJets'
    else:
        jets = 'updatedJetsPuppi'
        genJets = 'selectedGenJets'
        CHSjets = 'finalJets'

    process = setupSimonJets(process, jets, genJets,
                             CHSjets, False, 'ZMuMu',
                             'SimonJets', ak8)
    process = setupSimonJets(process, jets, genJets,
                             CHSjets, True, 'ZMuMu',
                             'SimonChargedJets', ak8)
    return process
