import FWCore.ParameterSet.Config as cms

ZMuMuFilter = cms.EDFilter('ZMuMuEventSelectionFilter',
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


def getZMuMuFilter(config, muons):
    Zmass = config['Zmass']
    Zwindow = config['ZmassWindow']
    ZmassMin = Zmass - Zwindow
    ZmassMax = Zmass + Zwindow
    return ZMuMuFilter.clone(
        leadPt = config['leadMuPt'],
        subPt = config['subMuPt'],
        minZmass = ZmassMin,
        maxZmass = ZmassMax,
        oppositeSign = config['requireOppositeSignMuons'],
        onlyCheckLeading = config['onlyCheckLeading'],
        src = muons
    )