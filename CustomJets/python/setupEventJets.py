import FWCore.ParameterSet.Config as cms
from SRothman.CustomJets.SimonJetTableProducer_cfi import *
from SRothman.CustomJets.EventJetProducer_cfi import *

from SRothman.CustomJets.systematics import variations

def setupGenEventJets(process,
                      genjets,
                      name):

    setattr(process, 'Gen'+name, GenEventJetProducer.clone(
        jetSrc = genjets,
        addCHSindex = False,
        verbose = False,
    ))

    setattr(process, 'Gen'+name+'Table', SimonJetTableProducer.clone(
        src = 'Gen'+name,
        name = 'Gen'+name,
        verbose=False,
    ))

    setattr(process, name+"MCTask", cms.Task(
        getattr(process, 'Gen'+name),
        getattr(process, 'Gen'+name+'Table')
    ))
    process.schedule.associate(getattr(process, name+'MCTask'))

    return process

def setupRecoEventJets(process,
                       jets,
                       CHSjets,
                       name,
                       syst):

    doCHS = len(CHSjets) > 0

    setattr(process, name, PatEventJetProducer.clone(
        jetSrc = jets,
        CHSsrc = CHSjets,
        addCHSindex = doCHS,
        CHSmatchDR = 0.4,
        verbose = False,
    ))
    getattr(process, name).selector.settings = variations[syst]

    setattr(process, name+"Preselection", cms.EDProducer("JetSelectionFlagTranslator",
        src = cms.InputTag(jets),
        target = cms.InputTag(name),
        map = cms.InputTag("preselectJetsAK8"),
        verbose = cms.int32(0)
    ))

    setattr(process, name+"OverlapVeto", cms.EDProducer("JetSelectionFlagTranslator",
        src = cms.InputTag(jets),
        target = cms.InputTag(name),
        map = cms.InputTag("overlapVetoJetsAK8"),
        verbose = cms.int32(0)
    ))

    setattr(process, name+'Table', SimonJetTableProducer.clone(
        src = name,
        name = name,
        verbose=False,
    ))

    setattr(process, name+"CHSTable", cms.EDProducer("CHSSumTableProducer",
        src = cms.InputTag(name),
        name = cms.string(name+"BK"),
        CHSsrc = cms.InputTag(CHSjets),
        verbose = cms.int32(0)
    ))

    setattr(process, name+'Task', cms.Task(
        getattr(process, name),
        getattr(process, name+'Preselection'),
        getattr(process, name+'OverlapVeto'),
        getattr(process, name+'Table'),
        getattr(process, name+'CHSTable')
    ))
    process.schedule.associate(getattr(process, name+'Task'))

    return process

def setupEventJets(process, 
                   jets, 
                   genjets,
                   CHSjets,
                   name,
                   syst,
                   isMC,
                   genOnly):
    if isMC:
        process = setupGenEventJets(process,
                                    genjets,
                                    name)
    if not genOnly:
        process = setupRecoEventJets(process,
                                     jets,
                                     CHSjets,
                                     name,
                                     syst)
    return process



