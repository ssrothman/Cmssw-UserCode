import FWCore.ParameterSet.Config as cms

from SRothman.Matching.TrackMatchProducer_cfi import TrackMatchProducer

def setupMatching(process, verbose=0,
                  ak8=True,
                  name='GenMatch',
                  reco = 'SimonJets',
                  gen = 'GenSimonJets',
                  naive=False):
                   
    setattr(process, name, 
        TrackMatchProducer.clone(
            recoJets = cms.InputTag(reco),
            genJets = cms.InputTag(gen),
        )
    )

    setattr(process, name+"Table", 
        cms.EDProducer("TrackMatchTableProducer",
            genname = cms.string(gen),
            reconame = cms.string(reco),
            recoJets = cms.InputTag(reco),
            genJets = cms.InputTag(gen),
            matches = cms.InputTag(name),
        )
    )

    setattr(process, name+"Task", cms.Task(
        getattr(process, name),
        getattr(process, name+"Table"),
    ))
    process.schedule.associate(getattr(process, name+"Task"))

    return process
