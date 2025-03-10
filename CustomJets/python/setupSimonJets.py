import FWCore.ParameterSet.Config as cms
from SRothman.CustomJets.SimonJetTableProducer_cfi import *
from SRothman.CustomJets.SimonJetProducer_cfi import *

def setupGenSimonJets(process,
                      genjets,
                      chargedOnly,
                      eventSelection,
                      name):

    doEventSel = len(eventSelection) > 0

    setattr(process, 'Gen'+name, GenSimonJetProducer.clone(
        jetSrc = genjets,
        eventSelection = eventSelection,
        doEventSelection = doEventSel,
        addCHSindex = False,
        verbose = False,
        onlyCharged = chargedOnly
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

def setupRecoSimonJets(process,
                       jets,
                       CHSjets,
                       chargedOnly,
                       eventSelection,
                       name,
                       ak8):
    doEventSel = len(eventSelection) > 0
    doCHS = len(CHSjets) > 0

    setattr(process, name, PatSimonJetProducer.clone(
        jetSrc = jets,
        eventSelection = eventSelection,
        doEventSelection = doEventSel,
        CHSsrc = CHSjets,
        addCHSindex = doCHS,
        CHSmatchDR = 0.4 if ak8 else 0.2,
        verbose = False,
        onlyCharged = chargedOnly
    ))

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

    setattr(process, name+'Task', cms.Task(
        getattr(process, name),
        getattr(process, name+'Preselection'),
        getattr(process, name+'OverlapVeto'),
        getattr(process, name+'Table'),
    ))
    process.schedule.associate(getattr(process, name+'Task'))

    return process

def setupSimonJets(process, 
                   jets, 
                   genjets,
                   CHSjets,
                   chargedOnly,
                   eventSelection,
                   name,
                   ak8,
                   isMC,
                   genOnly):
    if isMC:
        process = setupGenSimonJets(process,
                                    genjets,
                                    chargedOnly,
                                    eventSelection,
                                    name)
    if not genOnly:
        process = setupRecoSimonJets(process,
                                     jets,
                                     CHSjets,
                                     chargedOnly,
                                     eventSelection,
                                     name,
                                     ak8)
    return process



