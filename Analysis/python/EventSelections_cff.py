import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.CorrectedMuonProducer_cfi import CorrectedMuonProducer
from SRothman.Analysis.RoccoRValueMapProducer_cfi import RoccoRValueMapProducer
from SRothman.Analysis.ZMuMuEventSelectionFilter_cfi import RECOZMuMuFilter
from SRothman.Analysis.config.config import config

def setupEventSelections(process, isMC):
    process.RoccoR = RoccoRValueMapProducer.clone(
        src = cms.InputTag('linkedObjects', 'muons'),
        isMC = isMC
    )
    process.CorrectedMuons = CorrectedMuonProducer.clone(
        src = cms.InputTag('linkedObjects', 'muons'),
        RoccoR = cms.InputTag('RoccoR'),
        verbose = 0
    )

    muoncut = "abs(eta) < %0.2f && "%config['EventSelection']['MuEta'] + \
              "pt > %0.2f && "%config['EventSelection']['MuSubPt'] + \
              "passed('%s') && "%config['EventSelection']['MuID'] + \
              "passed('%s')" % config['EventSelection']['MuISO']
    muoncut = muoncut.encode('utf-8')

    process.SelectedMuons = cms.EDFilter(
        "MuonRefSelector",
        src = cms.InputTag("CorrectedMuons"),
        cut = cms.string(muoncut)
    )
    process.DiMuonFilter = cms.EDFilter(
        "CandViewCountFilter",
        src = cms.InputTag("SelectedMuons"),
        minNumber = cms.uint32(2)
    )
    process.ZMuMu = RECOZMuMuFilter.clone(
        src = cms.InputTag("SelectedMuons"),
        verbose = 0
    )

    process.METselector = cms.EDFilter(
        'CandViewSelector',
        src = cms.InputTag('slimmedMETsPuppi'),
        cut = cms.string('pt < %f' % config['EventSelection']['CoarsePuppiMETCut'])
    )
    process.METfilter = cms.EDFilter(
        'CandViewCountFilter',
        src = cms.InputTag('METselector'),
        minNumber = cms.uint32(1)
    )

    process.selections_path = cms.Path(
        process.RoccoR +
        process.CorrectedMuons +
        process.SelectedMuons +
        process.DiMuonFilter +
        process.ZMuMu + 
        process.METselector +
        process.METfilter
    )

    return process
