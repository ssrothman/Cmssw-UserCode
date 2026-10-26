import FWCore.ParameterSet.Config as cms

RecoZMuMuFilter = cms.EDFilter('LeafCandidateZMuMuFilter',
    leadPt = cms.double(-1),
    subPt = cms.double(-1),
    minZmass = cms.double(-1),
    maxZmass = cms.double(-1),
    oppositeSign = cms.bool(True),
    pdgIds = cms.vint32([13]),
    onlyCheckLeading = cms.bool(True),

    src = cms.InputTag(''),

    verbose = cms.int32(0),
    saveParticles = cms.bool(True),
)

GenZMuMuFilter = cms.EDFilter('PackedGenParticleZMuMuFilter',
    leadPt = cms.double(-1),
    subPt = cms.double(-1),
    minZmass = cms.double(-1),
    maxZmass = cms.double(-1),
    oppositeSign = cms.bool(True),
    pdgIds = cms.vint32([13]),
    onlyCheckLeading = cms.bool(True),

    src = cms.InputTag(''),

    verbose = cms.int32(0),
    saveParticles = cms.bool(True),
)

def getZMuMuFilter(config, muons, genlevel):
    Zmass = config['Zmass']
    Zwindow = config['ZmassWindow']
    ZmassMin = Zmass - Zwindow
    ZmassMax = Zmass + Zwindow

    theclass = GenZMuMuFilter if genlevel else RecoZMuMuFilter

    return theclass.clone(
        leadPt = config['leadMuPt'],
        subPt = config['subMuPt'],
        minZmass = ZmassMin,
        maxZmass = ZmassMax,
        oppositeSign = config['requireOppositeSignMuons'],
        onlyCheckLeading = config['onlyCheckLeading'],
        src = muons
    )