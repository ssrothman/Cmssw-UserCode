import FWCore.ParameterSet.Config as cms

def setupSimonJets(process, jets='updatedJetsPuppi', 
                   genjets = 'selectedGenJets',
                   CHSjets = 'finalJets',
                   chargedOnly=False,
                   eventSelection='ZMuMu',
                   name='SimonJets'):
    from SRothman.CustomJets.SimonJetTableProducer_cfi import *
    from SRothman.CustomJets.SimonJetProducer_cfi import *

    doEventSel = len(eventSelection) > 0
    doCHS = len(CHSjets) > 0
    
    setattr(process, name, PatSimonJetProducer.clone(
        jetSrc = jets,
        eventSelection = eventSelection,
        doEventSelection = doEventSel,
        CHSsrc = CHSjets,
        addCHSindex = doCHS,
        verbose = False,
        onlyCharged = chargedOnly
    ))
    setattr(process, 'Gen'+name, GenSimonJetProducer.clone(
        jetSrc = genjets,
        eventSelection = eventSelection,
        doEventSelection = doEventSel,
        addCHSindex = False,
        verbose = False,
        onlyCharged = chargedOnly
    ))
    setattr(process, name+'Table', SimonJetTableProducer.clone(
        src = name,
        name = name,
        verbose=False,
        isGen = False
    ))
    setattr(process, 'Gen'+name+'Table', SimonJetTableProducer.clone(
        src = 'Gen'+name,
        name = 'Gen'+name,
        verbose=False,
        isGen = True
    ))
    setattr(process, name+'Task', cms.Task(
        getattr(process, name),
        getattr(process, 'Gen'+name),
        getattr(process, name+'Table'),
        getattr(process, 'Gen'+name+'Table')
    ))
    process.schedule.associate(getattr(process, name+'Task'))
    return process



