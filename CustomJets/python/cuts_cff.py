import FWCore.ParameterSet.Config as cms

from SRothman.Analysis.config.config import config

recoThresholds = cms.PSet(
    EM0thresholds =   cms.vdouble(*config['ParticleCuts']['recoThresholds']['EM0']),
    HAD0thresholds =   cms.vdouble(*config['ParticleCuts']['recoThresholds']['HAD0']),
    HADCHthresholds =   cms.vdouble(*config['ParticleCuts']['recoThresholds']['HADCH']),
    ELEthresholds =   cms.vdouble(*config['ParticleCuts']['recoThresholds']['ELE']),
    MUthresholds =   cms.vdouble(*config['ParticleCuts']['recoThresholds']['MU']),

    etaRegions = cms.vdouble(*config['ParticleCuts']['recoThresholds']['EtaRegions'])
)

recoVtxCuts = cms.PSet(
    fromPVcut = cms.int32(config['ParticleCuts']['recoVtxCuts']['minFromPV']),
    puppiCut = cms.double(config['ParticleCuts']['recoVtxCuts']['minPuppiWt']),
    maxDZ = cms.double(config['ParticleCuts']['recoVtxCuts']['maxDZ']),
    maxDXY = cms.double(config['ParticleCuts']['recoVtxCuts']['maxDXY']),
)

genThresholds = cms.PSet(
    EM0thresholds =   cms.vdouble(*config['ParticleCuts']['genThresholds']['EM0']),
    HAD0thresholds =  cms.vdouble(*config['ParticleCuts']['genThresholds']['HAD0']),
    HADCHthresholds = cms.vdouble(*config['ParticleCuts']['genThresholds']['HADCH']),
    ELEthresholds =   cms.vdouble(*config['ParticleCuts']['genThresholds']['ELE']),
    MUthresholds =    cms.vdouble(*config['ParticleCuts']['genThresholds']['MU']),

    etaRegions = cms.vdouble(*config['ParticleCuts']['genThresholds']['EtaRegions'])
)

genVtxCuts = cms.PSet(
    fromPVcut = cms.int32(config['ParticleCuts']['genVtxCuts']['minFromPV']),
    puppiCut = cms.double(config['ParticleCuts']['genVtxCuts']['minPuppiWt']),
    maxDZ = cms.double(config['ParticleCuts']['genVtxCuts']['maxDZ']),
    maxDXY = cms.double(config['ParticleCuts']['genVtxCuts']['maxDXY']),
)
