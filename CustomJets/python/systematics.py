import FWCore.ParameterSet.Config as cms

systematics_parameters = cms.PSet(
    #particle energy scale uncertainties
    EM0scale = cms.double(0.03),
    HAD0scale = cms.double(0.05),
    CHscale = cms.double(0.01),

    #track efficiency uncertiainty
    trkDropProb = cms.double(0.03),
    #pT smearing for dropped tracks
    trkDropSmear = cms.double(0.10),

    #reconstruction thresholds
    EM0thresholds = cms.vdouble(0.0, 0.0, 0.0),   # DN, NOM, UP
    HAD0thresholds = cms.vdouble(0.0, 0.0, 0.0),  # DN, NOM, UP
    ELEthresholds = cms.vdouble(0.0, 0.0, 0.0),   # DN, NOM, UP
    MUthresholds = cms.vdouble(0.0, 0.0, 0.0),    # DN, NOM, UP
    HADCHthresholds = cms.vdouble(0.0, 0.0, 0.0), # DN, NOM, UP

    #vertexing selections
    minFromPV = cms.int32(0),
    minPuppiWt = cms.double(0.5),
    maxDZ = cms.double(0.2),
    maxDXY = cms.double(999999),
)

NOM = cms.PSet(
    EM0scale = cms.string("NOM"),
    HAD0scale = cms.string("NOM"),
    CHscale = cms.string("NOM"),

    trkDrop = cms.string("OFF"),

    EM0threshold = cms.string("NOM"),
    HAD0threshold = cms.string("NOM"),
    ELEthreshold = cms.string("NOM"),
    MUthreshold = cms.string("NOM"),
    HADCHthreshold = cms.string("NOM"),

    requireVertex = cms.string("ON"),
    
    applyPuppi = cms.bool(True),
    onlyCharged = cms.bool(True)
)

variations = {
    'NOM': NOM,
    'EM0_UP': NOM.clone(
        EM0scale = cms.string("UP"),
    ),
    'EM0_DN': NOM.clone(
        EM0scale = cms.string("DN"),
    ),
    'HAD0_UP': NOM.clone(
        HAD0scale = cms.string("UP"),
    ),
    'HAD0_DN': NOM.clone(
        HAD0scale = cms.string("DN"),
    ),
    'CH_UP': NOM.clone(
        CHscale = cms.string("UP"),
    ),
    'CH_DN': NOM.clone(
        CHscale = cms.string("DN"),
    ),
    'TRK_EFF': NOM.clone(
        trkDrop = cms.string("ON"),
    ),
    'HAD0_THRESH_UP': NOM.clone(
        HAD0threshold = cms.string("UP"),
    ),
    'HAD0_THRESH_DN': NOM.clone(
        HAD0threshold = cms.string("DN"),
    ),
    'EM0_THRESH_UP': NOM.clone(
        EM0threshold = cms.string("UP"),
    ),
    'EM0_THRESH_DN': NOM.clone(
        EM0threshold = cms.string("DN"),
    ),
    'ELE_THRESH_UP': NOM.clone(
        ELEthreshold = cms.string("UP"),
    ),
    'ELE_THRESH_DN': NOM.clone(
        ELEthreshold = cms.string("DN"),
    ),
    'MU_THRESH_UP': NOM.clone(
        MUthreshold = cms.string("UP"),
    ),
    'MU_THRESH_DN': NOM.clone(
        MUthreshold = cms.string("DN"),
    ),
    'HADCH_THRESH_UP': NOM.clone(
        HADCHthreshold = cms.string("UP"),
    ),
    'HADCH_THRESH_DN': NOM.clone(
        HADCHthreshold = cms.string("DN"),
    ),
}
