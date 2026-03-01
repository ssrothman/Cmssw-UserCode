import FWCore.ParameterSet.Config as cms

TrackMatchProducer = cms.EDProducer('TrackMatchProducer',
    recoJets = cms.InputTag(''),
    genJets = cms.InputTag(''),
    matcher = cms.PSet()
)

from SRothman.Analysis.util import pyval_to_cmsval

def track_matcher_from_config(config, recojets, genjets):
    return TrackMatchProducer.clone(
        recoJets = recojets,
        genJets = genjets,
        matcher = pyval_to_cmsval(config)
    )
