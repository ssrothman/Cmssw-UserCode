import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.CorrectedMuonProducer_cfi import CorrectedMuonProducer
from SRothman.Analysis.RoccoRValueMapProducer_cfi import RoccoRValueMapProducer
from SRothman.Analysis.ZMuMuEventSelectionFilter_cfi import RECOZMuMuFilter

RoccoR = RoccoRValueMapProducer.clone(
    src = cms.InputTag('linkedObjects', 'muons')
)
CorrectedMuons = CorrectedMuonProducer.clone(
    src = cms.InputTag('linkedObjects', 'muons'),
    RoccoR = cms.InputTag('RoccoR'),
    verbose = 0
)
SelectedMuons = cms.EDFilter(
    "MuonRefSelector",
    src = cms.InputTag("CorrectedMuons"),
    cut = cms.string("abs(eta) < 2.4 &&"
                     "pt > 10 &&"
                     "passed('CutBasedIdLoose') &&"
                     "passed('PFIsoLoose')")
)
DiMuonFilter = cms.EDFilter(
    "CandViewCountFilter",
    src = cms.InputTag("SelectedMuons"),
    minNumber = cms.uint32(2)
)
ZMuMu = RECOZMuMuFilter.clone(
    src = cms.InputTag("SelectedMuons"),
    verbose = 0
)

METselector = cms.EDFilter(
    'CandViewSelector',
    src = cms.InputTag('slimmedMETsPuppi'),
    cut = cms.string('pt < 50')
)
METfilter = cms.EDFilter(
    'CandViewCountFilter',
    src = cms.InputTag('METselector'),
    minNumber = cms.uint32(1)
)


selections_path = cms.Path(
    RoccoR +
    CorrectedMuons +
    SelectedMuons +
    DiMuonFilter +
    ZMuMu + 
    METselector +
    METfilter
)
