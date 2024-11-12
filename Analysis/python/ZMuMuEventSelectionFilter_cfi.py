import FWCore.ParameterSet.Config as cms
from SRothman.Analysis.config.config import config

Zmass = config['EventSelection']['Zmass']
ZmassWindow = config['EventSelection']['ZmassWindow']
ZmassMin = Zmass - ZmassWindow
ZmassMax = Zmass + ZmassWindow

GENZMuMuFilter = cms.EDFilter('ZMuMuEventSelectionFilter',
    leadPt = cms.double(config['EventSelection']['MuLeadPt']),
    subPt = cms.double(config['EventSelection']['MuSubPt']),
    minZmass = cms.double(ZmassMin),
    maxZmass = cms.double(ZmassMax),
    oppositeSign = cms.bool(config['EventSelection']['RequireOppositeSignMuons']),
    pdgIds = cms.vint32([13]),
    onlyCheckLeading = cms.bool(config['EventSelection']['OnlyCheckLeadingMuons']),

    src = cms.InputTag('genParticles'),

    verbose = cms.int32(0),
    saveParticles = cms.bool(True),
)

RECOZMuMuFilter = cms.EDFilter('ZMuMuEventSelectionFilter',
    leadPt = cms.double(config['EventSelection']['MuLeadPt']),
    subPt = cms.double(config['EventSelection']['MuSubPt']),
    minZmass = cms.double(ZmassMin),
    maxZmass = cms.double(ZmassMax),
    oppositeSign = cms.bool(config['EventSelection']['RequireOppositeSignMuons']),
    pdgIds = cms.vint32([13]),
    onlyCheckLeading = cms.bool(config['EventSelection']['OnlyCheckLeadingMuons']),

    src = cms.InputTag('linkedObjects','muons'),

    verbose = cms.int32(0),
    saveParticles = cms.bool(True),
)
