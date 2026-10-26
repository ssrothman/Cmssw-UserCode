import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.CorrectedMuonProducer_cfi import CorrectedMuonProducer
from SRothman.Analysis.RoccoRValueMapProducer_cfi import RoccoRValueMapProducer
from SRothman.Analysis.ZMuMuEventSelectionFilter_cfi import getZMuMuFilter
from SRothman.Analysis.addParticlesTable_cff import addParticlesTable, addCollectionIndices

def setupEventSelections(process,
                         muons,
                         isMC, 
                         config,
                         nameprefix,
                         PAT,
                         genlevel):

    modules = []

    if not genlevel:
        #setup Rochester corrections table
        setattr(process, nameprefix + 'RoccoR', RoccoRValueMapProducer.clone(
            src = cms.InputTag(muons),
            isMC = isMC
        ))
        modules.append(getattr(process, nameprefix + 'RoccoR'))

        setattr(process.muonTable.externalVariables, nameprefix + 'RoccoR', cms.PSet(
            compression = cms.string('none'),
            doc = cms.string("Rochester correction factor"),
            mcOnly = cms.bool(False),
            precision = cms.int32(-1),
            src = cms.InputTag(nameprefix + 'RoccoR'),
            type = cms.string('float')
        ))

        if PAT:
            selectorclass = "PATMuonSelector"
        else:
            raise NotImplementedError("reco::muon selector not implemented")
    else:
        if PAT:
            selectorclass = "PATPackedGenParticleSelector"
        else:
            selectorclass = "GenParticleSelector"
        
    muoncut = 'abs(pdgId) == 13 && '
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

    setattr(process, nameprefix + 'SelectedMuons', cms.EDFilter(
        selectorclass,
        src = cms.InputTag(muons),
        cut = cms.string(muoncut)
    ))
    modules.append(getattr(process, nameprefix + 'SelectedMuons'))

    setattr(process, nameprefix + 'DiMuonFilter', cms.EDFilter(
        "CandViewCountFilter",
        src = cms.InputTag(nameprefix + "SelectedMuons"),
        minNumber = cms.uint32(2)
    ))
    modules.append(getattr(process, nameprefix + 'DiMuonFilter'))

    setattr(process, nameprefix + 'ZMuMu', 
        getZMuMuFilter(
            config, 
            cms.InputTag(nameprefix + "SelectedMuons"),
            genlevel = genlevel
        )
    )
    modules.append(getattr(process, nameprefix + 'ZMuMu'))

    if config['maxMET'] > 0:
        setattr(process, nameprefix + 'METselector', cms.EDFilter(
            'CandViewSelector',
            src = cms.InputTag('slimmedMETsPuppi'),
            cut = cms.string('pt < %f' % config['maxMET'])
        ))
        setattr(process, nameprefix + 'METfilter', cms.EDFilter(
            'CandViewCountFilter',
            src = cms.InputTag(nameprefix + 'METselector'),
            minNumber = cms.uint32(1)
        ))
        modules.append(getattr(process, nameprefix + 'METselector'))
        modules.append(getattr(process, nameprefix + 'METfilter'))

    paths = []
    setattr(process, nameprefix + 'selections_path', cms.Path())
    paths.append(getattr(process, nameprefix + 'selections_path'))
    for imodule in range(len(modules)):
        module = modules[imodule]
        getattr(process, nameprefix + 'selections_path').__iadd__(module)

        setattr(process, module.label() + 'Path', cms.Path())
        for i in range(imodule + 1):
            getattr(process, module.label() + 'Path').__iadd__(modules[i])
        paths.append(getattr(process, module.label() + 'Path'))

    return process, paths

def addSelectionTables(process, nameprefix, muons, genlevel):
    process = addParticlesTable(
        process,
        nameprefix + 'ZMuMu:daughters',
        nameprefix + 'ZMuMuMuons',
        singleton=False
    )
    process = addCollectionIndices(
        process,
        nameprefix + 'ZMuMu:daughters',
        nameprefix + 'ZMuMuMuons',
        muons,
        genlevel
    )
    process = addParticlesTable(
        process,
        nameprefix + 'ZMuMu:Z',
        nameprefix + 'ZMuMuZ',
        singleton=False
    )
    return process