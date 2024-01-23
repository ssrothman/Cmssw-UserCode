import FWCore.ParameterSet.Config as cms

recoThresholds = cms.PSet(
    EM0thresholds =   cms.vdouble(0.0, 0.0, 0.0),
    HAD0thresholds =  cms.vdouble(0.0, 0.0, 0.0),
    HADCHthresholds = cms.vdouble(0.0, 0.0, 0.0),
    ELEthresholds =   cms.vdouble(0.0, 0.0, 0.0),
    MUthresholds =    cms.vdouble(0.0, 0.0, 0.0),

    etaRegions = cms.vdouble(0.0, 1.0, 2.0, 3.0) 
)

genThresholds = cms.PSet(
    EM0thresholds =   cms.vdouble(0.0),
    HAD0thresholds =  cms.vdouble(0.0),
    HADCHthresholds = cms.vdouble(0.0),
    ELEthresholds =   cms.vdouble(0.0),
    MUthresholds =    cms.vdouble(0.0),

    etaRegions = cms.vdouble(0.0, 10.0)
)
