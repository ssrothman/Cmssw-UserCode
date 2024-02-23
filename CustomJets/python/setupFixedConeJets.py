import FWCore.ParameterSet.Config as cms

def setupFixedConeJets(process, parts='packedPFCandidates',
                       genparts='packedGenParticles',
                       onlyCharged=False,
                       eventSelection='ZMuMu',
                       name='FixedConeJets',
                       random=True,
                       ak8=False,
                       isMC=True):
    from SRothman.CustomJets.FixedConeJetProducer_cfi import *
    from SRothman.CustomJets.RandomConeCoordsProducer_cfi import *
    from SRothman.CustomJets.ControlRegionCoordsProducer_cfi import *

    from SRothman.CustomJets.SimonJetTableProducer_cfi import *

    doEventSel = len(eventSelection) > 0

    if random:
        setattr(process, name+"Coords", RandomConeCoordsProducer.clone(
            verbose=False
        ))
    else:
        setattr(process, name+"Coords", ControlRegionCoordsProducer.clone(
            verbose=False,
            muonSrc='slimmedMuons',
        ))

    setattr(process, name, RecoFixedConeJetProducer.clone(
        onlyCharged=onlyCharged,
        eventSelection=eventSelection,
        doEventSelection=doEventSel,
        particles=parts,
        coords = name+"Coords",
        verbose=False,
        conesize=0.8 if ak8 else 0.4
    ))
    setattr(process, name+'Table', SimonJetTableProducer.clone(
        src=name,
        name=name,
        verbose=False,
        isGen=False
    ))
    setattr(process, name+'Task', cms.Task(
        getattr(process, name+"Coords"),
        getattr(process, name),
        getattr(process, name+'Table'),
    ))
    process.schedule.associate(getattr(process, name+'Task'))

    if isMC:
        setattr(process, 'Gen'+name, GenFixedConeJetProducer.clone(
            onlyCharged=onlyCharged,
            eventSelection=eventSelection,
            doEventSelection=doEventSel,
            particles=genparts,
            coords = name+"Coords",
            conesize=0.8 if ak8 else 0.4,
            verbose=False,
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
