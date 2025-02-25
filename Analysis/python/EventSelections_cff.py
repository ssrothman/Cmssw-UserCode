import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.CorrectedMuonProducer_cfi import CorrectedMuonProducer
from SRothman.Analysis.RoccoRValueMapProducer_cfi import RoccoRValueMapProducer
from SRothman.Analysis.ZMuMuEventSelectionFilter_cfi import RECOZMuMuFilter
from SRothman.Analysis.config.config import config

def setupEventSelections(process, isMC, 
                         genmuons=False,
                         skipMET=False):
    print("")
    print(" ------------ SETUP EVENT SELECTIONS ------------ ")
    print("")

    if not genmuons:
        print("RECO Muons")
        process.RoccoR = RoccoRValueMapProducer.clone(
            src = cms.InputTag('linkedObjects', 'muons'),
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

        process.CorrectedMuons = CorrectedMuonProducer.clone(
            src = cms.InputTag('linkedObjects', 'muons'),
            RoccoR = cms.InputTag('RoccoR'),
            verbose = 0
        )
        print("YES ROCCOR")

        muoncut = "abs(eta) < %0.2f && "%config['EventSelection']['MuEta'] + \
                  " pt > %0.2f && "%config['EventSelection']['MuSubPt'] + \
                  " passed('%s') && "%config['EventSelection']['MuID'] + \
                  " passed('%s') && " % config['EventSelection']['MuISO'] + \
                  " abs(dB('PVDZ')) < 0.5 && abs(dB('PV2D')) < 0.2" 

        print("Muon cut: %s" % muoncut)

        process.SelectedMuons = cms.EDFilter(
            "PATMuonRefSelector",
            src = cms.InputTag("CorrectedMuons"),
            cut = cms.string(muoncut)
        )
        muoncut = muoncut.encode('utf-8')

    else:
        print("GEN Muons")
        print("NO ROCCOR")

        muoncut = 'abs(eta) < %0.2f && '%config['EventSelection']['MuEta'] + \
                  ' pt > %0.2f &&'%config['EventSelection']['MuSubPt'] + \
                  ' abs(pdgId) == 13 &&' + \
                  ' status == 1'

        print("Muon cut: %s" % muoncut)

        process.SelectedMuons = cms.EDFilter(
            'GenParticleSelector',
            src = cms.InputTag('genParticles'),
            cut = cms.string(muoncut)
        )

    process.DiMuonFilter = cms.EDFilter(
        "CandViewCountFilter",
        src = cms.InputTag("SelectedMuons"),
        minNumber = cms.uint32(2)
    )
    print("Selecting events with >= 2 muons")

    process.ZMuMu = RECOZMuMuFilter.clone(
        src = cms.InputTag("SelectedMuons"),
        verbose = 0
    )
    print("Running ZMuMu filter")

    if not skipMET:
        process.METselector = cms.EDFilter(
            'CandViewSelector',
            src = cms.InputTag('slimmedMETsPuppi'),
            cut = cms.string('pt < %f' % config['EventSelection']['CoarsePuppiMETCut'])
        )
        print("Selecting events with MET < %f" % config['EventSelection']['CoarsePuppiMETCut'])
        process.METfilter = cms.EDFilter(
            'CandViewCountFilter',
            src = cms.InputTag('METselector'),
            minNumber = cms.uint32(1)
        )
    else:
        print("NO MET cut")

    if not genmuons and not skipMET:
        process.selections_path = cms.Path(
            process.RoccoR +
            process.CorrectedMuons +
            process.SelectedMuons +
            process.DiMuonFilter +
            process.ZMuMu + 
            process.METselector +
            process.METfilter
        )
    elif not genmuons:
        process.selections_path = cms.Path(
            process.RoccoR +
            process.CorrectedMuons +
            process.SelectedMuons +
            process.DiMuonFilter +
            process.ZMuMu
        )
    elif not skipMET:
        process.selections_path = cms.Path(
            process.SelectedMuons +
            process.DiMuonFilter +
            process.ZMuMu + 
            process.METselector +
            process.METfilter
        )
    else:
        process.selections_path = cms.Path(
            process.SelectedMuons +
            process.DiMuonFilter +
            process.ZMuMu
        )

    print("")
    print(" ------------ END SETUP EVENT SELECTIONS ------------ ")
    print("")

    return process
