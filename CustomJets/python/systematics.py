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
    EM0thresholds = cms.vdouble(0.0, 0.0, 0.0),   # NOM, UP, DN
    HAD0thresholds = cms.vdouble(0.0, 0.0, 0.0),  # NOM, UP, DN
    ELEthresholds = cms.vdouble(0.0, 0.0, 0.0),   # NOM, UP, DN
    MUthresholds = cms.vdouble(0.0, 0.0, 0.0),    # NOM, UP, DN
    HADCHthresholds = cms.vdouble(0.0, 0.0, 0.0), # NOM, UP, DN

    #vertexing selections
    fromPVcut = cms.int32(0),
    puppiCut = cms.double(0.5),
    maxDZ = cms.double(0.1),
    maxDXY = cms.double(0.1),
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

    requireVertex = cms.bool(True),
    
    applyPuppi = cms.bool(True),
    onlyCharged = cms.bool(True)
)

EM0_UP = NOM.clone(
    EM0scale = cms.string("UP"),
)

EM0_DOWN = NOM.clone(
    EM0scale = cms.string("DOWN"),
)

HAD0_UP = NOM.clone(
    HAD0scale = cms.string("UP"),
)

HAD0_DOWN = NOM.clone(
    HAD0scale = cms.string("DOWN"),
)

CH_UP = NOM.clone(
    CHscale = cms.string("UP"),
)

CH_DOWN = NOM.clone(
    CHscale = cms.string("DOWN"),
)

TRK_EFF = NOM.clone(
    trkDrop = cms.string("ON"),
)

HAD0_THRESH_UP = NOM.clone(
    HAD0threshold = cms.string("UP"),
)

HAD0_THRESH_DOWN = NOM.clone(
    HAD0threshold = cms.string("DOWN"),
)

EM0_THRESH_UP = NOM.clone(
    EM0threshold = cms.string("UP"),
)

EM0_THRESH_DOWN = NOM.clone(
    EM0threshold = cms.string("DOWN"),
)

ELE_THRESH_UP = NOM.clone(
    ELEthreshold = cms.string("UP"),
)

ELE_THRESH_DOWN = NOM.clone(
    ELEthreshold = cms.string("DOWN"),
)

MU_THRESH_UP = NOM.clone(
    MUthreshold = cms.string("UP"),
)

MU_THRESH_DOWN = NOM.clone(
    MUthreshold = cms.string("DOWN"),
)

HADCH_THRESH_UP = NOM.clone(
    HADCHthreshold = cms.string("UP"),
)

HADCH_THRESH_DOWN = NOM.clone(
    HADCHthreshold = cms.string("DOWN"),
)
