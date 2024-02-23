import FWCore.ParameterSet.Config as cms

def setupShadowJets(process, jets='SimonJets',
                    genjets='GenSimonJets',
                    parts='packedPFCandidates',
                    genParts='packedGenParticles', 
                    chargedOnly=False,
                    eventSelection='ZMuMu',
                    name='ShadowJets',
                    isMC=True):
    from SRothman.CustomJets.ShadowJetProducer_cfi import *
    from SRothman.CustomJets.SimonJetTableProducer_cfi import *

    doEventSel = len(eventSelection) > 0

    setattr(process, name, ShadowJetProducer.clone(
        jetSrc = genjets,
        partSrc = parts,
        eventSelection = eventSelection,
        doEventSelection = doEventSel,
        verbose = False,
        onlyCharged = chargedOnly
    ))
    if isMC:
        setattr(process, 'Gen'+name, GenShadowJetProducer.clone(
            jetSrc = jets,
            partSrc = genParts,
            eventSelection = eventSelection,
            doEventSelection = doEventSel,
            verbose = False,
            onlyCharged = chargedOnly
        ))
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

    setattr(process, name+'Task', cms.Task(
        getattr(process, name),
        getattr(process, name+'Table'),
    ))
    process.schedule.associate(getattr(process, name+'Task'))
    if isMC:
        setattr(process, name+'MCTask', cms.Task(
            getattr(process, 'Gen'+name),
            getattr(process, 'Gen'+name+'Table')
        ))
        process.schedule.associate(getattr(process, name+'MCTask'))
    return process

