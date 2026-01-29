import FWCore.ParameterSet.Config as cms

from SRothman.Matching.TrackMatchProducer_cfi import track_matcher_from_config

def setupMatching(process, 
                  config,
                  name,
                  reco,
                  gen):
                   
    setattr(process, name, 
        track_matcher_from_config(
            recojets = reco,
            genjets = gen,
            config = config
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