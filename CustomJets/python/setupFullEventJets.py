import FWCore.ParameterSet.Config as cms

def setupFullEventJets(process, parts='packedPFCandidates',
                       genparts='packedGenParticles',
                       onlyCharged=False,
                       eventSelection='ZMuMu',
                       name='FullEventJets',
                       isMC=True):
    from SRothman.CustomJets.FullEventJetProducer_cfi import *
    from SRothman.CustomJets.SimonJetTableProducer_cfi import *

    doEventSel = len(eventSelection) > 0

    setattr(process, name, RecoFullEventJetProducer.clone(
        onlyCharged=onlyCharged,
        eventSelection=eventSelection,
        doEventSelection=doEventSel,
        partSrc=parts,
        verbose=False
    ))
    setattr(process, name+'Table', SimonJetTableProducer.clone(
        src=name,
        name=name,
        verbose=False,
        isGen=False
    ))
    setattr(process, name+'Task', cms.Task(
        getattr(process, name),
        getattr(process, name+'Table'),
    ))
    process.schedule.associate(getattr(process, name+'Task'))

    if isMC:
        setattr(process, 'Gen'+name, GenFullEventJetProducer.clone(
            onlyCharged=onlyCharged,
            eventSelection=eventSelection,
            doEventSelection=doEventSel,
            partSrc=genparts,
            verbose=False
        ))
        setattr(process, 'Gen'+name+'Table', SimonJetTableProducer.clone(
            src='Gen'+name,
            name='Gen'+name,
            verbose=False,
            isGen=True
        ))
        setattr(process, 'Gen'+name+'Task', cms.Task(
            getattr(process, 'Gen'+name),
            getattr(process, 'Gen'+name+'Table'),
        ))
        process.schedule.associate(getattr(process, 'Gen'+name+'Task'))

    return process
