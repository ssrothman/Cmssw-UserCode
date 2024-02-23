import FWCore.ParameterSet.Config as cms

from SRothman.CustomJets.setupPuppiJets import *
from SRothman.CustomJets.setupSimonJets import *
from SRothman.CustomJets.setupShadowJets import *
from SRothman.CustomJets.setupFullEventJets import *
from SRothman.CustomJets.setupFixedConeJets import *

def setupCustomJets(process, verbose=False, ak8 = False, 
                    isMC=True):

    parts = 'packedPFCandidates'
    genParts = 'packedGenParticlesForJetsNoNu'

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
                             'SimonJets', ak8, isMC=isMC)
    process = setupSimonJets(process, jets, genJets,
                             CHSjets, True, 'ZMuMu',
                             'ChargedSimonJets', ak8, isMC=isMC)

    process = setupFullEventJets(process, parts, genParts,
                                 False, 'ZMuMu',
                                 'FullEventJets', isMC=isMC)
    process = setupFullEventJets(process, parts, genParts,
                                 True, 'ZMuMu',
                                 'ChargedFullEventJets', isMC=isMC)

    process = setupFixedConeJets(process, parts, genParts,
                                 False, 'ZMuMu',
                                 'RandomConeJets', 
                                 random=True, ak8=ak8, isMC=isMC)
    process = setupFixedConeJets(process, parts, genParts,
                                 True, 'ZMuMu',
                                 'ChargedRandomConeJets',
                                 random=True, ak8=ak8, isMC=isMC)

    process = setupFixedConeJets(process, parts, genParts,
                                 False, 'ZMuMu',
                                 'ControlConeJets',
                                 random=False, ak8=ak8, isMC=isMC)
    process = setupFixedConeJets(process, parts, genParts,
                                 True, 'ZMuMu',
                                 'ChargedControlConeJets',
                                 random=False, ak8=ak8, isMC=isMC)
    return process
