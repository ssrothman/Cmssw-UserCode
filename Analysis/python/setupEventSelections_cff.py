import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.CorrectedMuonProducer_cfi import CorrectedMuonProducer
from SRothman.Analysis.RoccoRValueMapProducer_cfi import RoccoRValueMapProducer
from SRothman.Analysis.ZMuMuEventSelectionFilter_cfi import getZMuMuFilter

def setupEventSelections(process,
                         muons,
                         isMC, 
                         config,
                         genmuons=False):

    if not genmuons:
        #setup Rochester corrections table
        process.RoccoR = RoccoRValueMapProducer.clone(
            src = cms.InputTag(muons),
            isMC = isMC
        )

        process.muonTable.externalVariables.RoccoR = cms.PSet(
            compression = cms.string('none'),
            doc = cms.string("Rochester correction factor"),
            mcOnly = cms.bool(False),
            precision = cms.int32(-1),
            src = cms.InputTag("RoccoR"),
            type = cms.string('float')
        )

        selectorclass = "PATMuonSelector"
    else:
        selectorclass = "GenParticleSelector"
        
    muoncut = ''
    if config['maxMuEta'] > 0:
        muoncut += "abs(eta) < %0.2f && "%config['maxMuEta']
    if config['subMuPt'] > 0:
        muoncut += " pt > %0.2f && "%config['subMuPt']
    if config['muID'] not in ['', 'none']:
        muoncut += " passed('%s') && "%config['muID']
    if config['muISO'] not in ['', 'none']:
        muoncut += " passed('%s') && "%config['muISO']
    if config['muDZ'] > 0:
        muoncut += " abs(dB('PVDZ')) < %0.2f && "%config['muDZ']
    if config['muDXY'] > 0:
        muoncut += " abs(dB('PV2D')) < %0.2f && "%config['muDXY']
    
    if muoncut.endswith(' && '):
        muoncut = muoncut[:-4]  # remove trailing ' && '

    process.SelectedMuons = cms.EDFilter(
        selectorclass,
        src = cms.InputTag(muons),
        cut = cms.string(muoncut)
    )

    process.DiMuonFilter = cms.EDFilter(
        "CandViewCountFilter",
        src = cms.InputTag("SelectedMuons"),
        minNumber = cms.uint32(2)
    )

    process.ZMuMu = getZMuMuFilter(config, cms.InputTag("SelectedMuons"))

    if config['maxMET'] > 0:
        process.METselector = cms.EDFilter(
            'CandViewSelector',
            src = cms.InputTag('slimmedMETsPuppi'),
            cut = cms.string('pt < %f' % config['maxMET'])
        )
        process.METfilter = cms.EDFilter(
            'CandViewCountFilter',
            src = cms.InputTag('METselector'),
            minNumber = cms.uint32(1)
        )

    path = []
    if not genmuons:
        path.append(process.RoccoR)

    path.append(process.SelectedMuons)
    path.append(process.DiMuonFilter)
    path.append(process.ZMuMu)
    if config['maxMET'] > 0:
        path.append(process.METselector)
        path.append(process.METfilter)

    process.selections_path = cms.Path()
    for module in path:
        process.selections_path += module

    return process
