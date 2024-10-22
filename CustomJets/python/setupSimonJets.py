import FWCore.ParameterSet.Config as cms

def setupSimonJets(process, jets='updatedJetsPuppi', 
                   genjets = 'selectedGenJets',
                   CHSjets = 'finalJets',
                   chargedOnly=False,
                   eventSelection='ZMuMu',
                   name='SimonJets',
                   ak8=False,
                   isMC=True,
                   genOnly=False):

    from SRothman.CustomJets.SimonJetTableProducer_cfi import *
    from SRothman.CustomJets.SimonJetProducer_cfi import *

    doEventSel = len(eventSelection) > 0
    doCHS = len(CHSjets) > 0
    
    if not genOnly:
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
    if isMC:
        setattr(process, 'Gen'+name, GenSimonJetProducer.clone(
            jetSrc = genjets,
            eventSelection = eventSelection,
            doEventSelection = doEventSel,
            addCHSindex = False,
            verbose = False,
            onlyCharged = chargedOnly
        ))
    if not genOnly:
        setattr(process, name+'Table', SimonJetTableProducer.clone(
            src = name,
            name = name,
            verbose=False,
            isGen = False
        ))
    if isMC:
        setattr(process, 'Gen'+name+'Table', SimonJetTableProducer.clone(
            src = 'Gen'+name,
            name = 'Gen'+name,
            verbose=False,
            isGen = True
        ))
    if not genOnly:
        setattr(process, name+'Task', cms.Task(
            getattr(process, name),
            getattr(process, name+'Table'),
        ))
        process.schedule.associate(getattr(process, name+'Task'))
    if isMC:
        setattr(process, name+"MCTask", cms.Task(
            getattr(process, 'Gen'+name),
            getattr(process, 'Gen'+name+'Table')
        ))
        process.schedule.associate(getattr(process, name+'MCTask'))
    return process



