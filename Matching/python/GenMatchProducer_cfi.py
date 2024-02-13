import FWCore.ParameterSet.Config as cms

# goes [EM0, HAD0, HADCH, ELE, MU]

GenMatchProducer = cms.EDProducer("GenMatchProducer",
    jetMatchingDR = cms.double(0.2),

    clipval = cms.double(0.01),

    spatialLoss = cms.int32(0),

    PUpt0s = cms.vdouble(1.0, 1.0, 
                         1.0, 1.0, 1.0),
    PUexps = cms.vdouble(4.0, 4.0, 
                         4.0, 4.0, 4.0),
    PUpenalties = cms.vdouble(2.0e12, 2.0e12, 
                              2.0e12, 2.0e12, 2.0e12),

    uncertainty = cms.string("Standard"),

    softflavorfilters = cms.vstring(
        "AnyPhoton", "AnyNeutralHadron", 
        "AnyCharged", "AnyCharged", "AnyCharged"),
    hardflavorfilters = cms.vstring(
        "AnyNeutral", "AnyNeutralHadron", 
        "AnyCharged", "AnyCharged", "AnyCharged"),
    filterthresholds = cms.vdouble(3.0, 0.0, 0.0, 0.0, 0.0),

    chargefilters = cms.vstring(
        'ChargeSign', 'ChargeSign',
        'ChargeSign', 'ChargeSign', 'ChargeSign'),

    dRfilters = cms.vstring(
        "Fixed", "Fixed", 
        'Tracking', 'Tracking', 'Tracking'),

    prefitters = cms.vstring(
        "Float", "Float", 
        "Best", "Best", "Best"),

    refiner = cms.string("OneGenOneRecoPerType"),
    dropGenFilter = cms.string("NONE"),
    dropRecoFilter = cms.string("NONE"),

    recoverLostTracks = cms.bool(True),
    propagateLostTracks = cms.bool(True),
    HADCHrecoverThresholds = cms.vdouble(5.0, 5.0, 5.0),
    ELErecoverThresholds = cms.vdouble(5.0, 5.0, 5.0),

    recoverLostHAD0 = cms.bool(False),
    HAD0recoverThresholds = cms.vdouble(0.0, 0.0, 0.0),

    EMstochastic = cms.vdouble(0.17, 0.18, 0.80),
    EMconstant = cms.vdouble(0.0074, 0.0253, 0.0253),
    ECALgranularityEta = cms.vdouble(0.05, 0.05, 0.07),
    ECALgranularityPhi = cms.vdouble(0.05, 0.05, 0.07),
    ECALEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    HADstochastic = cms.vdouble(1.63, 3.90, 6.44),
    HADconstant = cms.vdouble(0.21, 0.14, 0.10),
    HCALgranularityEta = cms.vdouble(0.10, 0.10, 0.15),
    HCALgranularityPhi = cms.vdouble(0.10, 0.10, 0.15),
    HCALEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    CHlinear = cms.vdouble(0.000069, 0.000072, 0.000072),
    CHconstant = cms.vdouble(0.0076, 0.014, 0.018),
    CHMSeta = cms.vdouble(0.0000, 0.0000, 0.0000),
    CHMSphi = cms.vdouble(0.0000, 0.0000, 0.0000),
    CHangularEta = cms.vdouble(0.001, 0.001, 0.001),
    CHangularPhi = cms.vdouble(0.001, 0.001, 0.001),
    trkEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    EM0thresholds =   cms.vdouble(0.0, 0.0, 0.0),
    HAD0thresholds =  cms.vdouble(0.0, 0.0, 0.0),
    HADCHthresholds = cms.vdouble(0.0, 0.0, 0.0),
    ELEthresholds =   cms.vdouble(0.0, 0.0, 0.0),
    MUthresholds =    cms.vdouble(0.0, 0.0, 0.0),

    EM0constDR = cms.vdouble(0.050, 0.050, 0.050),
    EM0floatDR = cms.vdouble(0.000, 0.000, 0.000),
    EM0capDR =   cms.vdouble(0.000, 0.000, 0.000),

    HAD0constDR = cms.vdouble(0.100, 0.100, 0.150),
    HAD0floatDR = cms.vdouble(0.000, 0.000, 0.000),
    HAD0capDR =   cms.vdouble(0.000, 0.000, 0.000),

    HADCHconstDR = cms.vdouble(0.002, 0.002, 0.003),
    HADCHfloatDR = cms.vdouble(0.010, 0.010, 0.015),
    HADCHcapDR =   cms.vdouble(0.050, 0.050, 0.070),

    ELEconstDR = cms.vdouble(0.002, 0.002, 0.003),
    ELEfloatDR = cms.vdouble(0.010, 0.010, 0.015),
    ELEcapDR =   cms.vdouble(0.050, 0.050, 0.070),

    MUconstDR = cms.vdouble(0.002, 0.002, 0.003),
    MUfloatDR = cms.vdouble(0.010, 0.010, 0.015),
    MUcapDR =   cms.vdouble(0.050, 0.050, 0.070),

    maxReFit = cms.uint32(50),

    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("GenSimonJets"),

    verbose = cms.int32(1)
)

NaiveMatchProducer = cms.EDProducer("GenMatchProducer",
    jetMatchingDR = cms.double(0.2),

    clipval = cms.double(0.01),

    spatialLoss = cms.int32(0),

    PUpt0s = cms.vdouble(1.0, 1.0, 
                         1.0, 1.0, 1.0),
    PUexps = cms.vdouble(4.0, 4.0, 
                         4.0, 4.0, 4.0),
    PUpenalties = cms.vdouble(2.0e12, 2.0e12, 
                              2.0e12, 2.0e12, 2.0e12),

    uncertainty = cms.string("Standard"),

    softflavorfilters = cms.vstring(
        "Any", "Any", 
        "Any", "Any", "Any"),
    hardflavorfilters = cms.vstring(
        "Any", "Any", 
        "Any", "Any", "Any"),
    filterthresholds = cms.vdouble(0.0, 0.0, 0.0, 0.0, 0.0),

    chargefilters = cms.vstring(
        'ChargeSign', 'ChargeSign',
        'ChargeSign', 'ChargeSign', 'ChargeSign'),

    dRfilters = cms.vstring(
        "Fixed", "Fixed", 
        'Fixed', 'Fixed', 'Fixed'),

    prefitters = cms.vstring(
        "Best", "Best", 
        "Best", "Best", "Best"),

    refiner = cms.string("OneGenOneReco"),
    dropGenFilter = cms.string("NONE"),
    dropRecoFilter = cms.string("NONE"),

    recoverLostTracks = cms.bool(False),
    propagateLostTracks = cms.bool(False),
    HADCHrecoverThresholds = cms.vdouble(5.0, 5.0, 5.0),
    ELErecoverThresholds = cms.vdouble(5.0, 5.0, 5.0),

    recoverLostHAD0 = cms.bool(False),
    HAD0recoverThresholds = cms.vdouble(0.0, 0.0, 0.0),

    EMstochastic = cms.vdouble(0.17, 0.18, 0.80),
    EMconstant = cms.vdouble(0.0074, 0.0253, 0.0253),
    ECALgranularityEta = cms.vdouble(0.05, 0.05, 0.07),
    ECALgranularityPhi = cms.vdouble(0.05, 0.05, 0.07),
    ECALEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    HADstochastic = cms.vdouble(1.63, 3.90, 6.44),
    HADconstant = cms.vdouble(0.21, 0.14, 0.10),
    HCALgranularityEta = cms.vdouble(0.10, 0.10, 0.15),
    HCALgranularityPhi = cms.vdouble(0.10, 0.10, 0.15),
    HCALEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    CHlinear = cms.vdouble(0.000069, 0.000072, 0.000072),
    CHconstant = cms.vdouble(0.0076, 0.014, 0.018),
    CHMSeta = cms.vdouble(0.0000, 0.0000, 0.0000),
    CHMSphi = cms.vdouble(0.0000, 0.0000, 0.0000),
    CHangularEta = cms.vdouble(0.001, 0.001, 0.001),
    CHangularPhi = cms.vdouble(0.001, 0.001, 0.001),
    trkEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    EM0thresholds =   cms.vdouble(0.0, 0.0, 0.0),
    HAD0thresholds =  cms.vdouble(0.0, 0.0, 0.0),
    HADCHthresholds = cms.vdouble(0.0, 0.0, 0.0),
    ELEthresholds =   cms.vdouble(0.0, 0.0, 0.0),
    MUthresholds =    cms.vdouble(0.0, 0.0, 0.0),

    EM0constDR = cms.vdouble(0.050, 0.050, 0.050),
    EM0floatDR = cms.vdouble(0.000, 0.000, 0.000),
    EM0capDR =   cms.vdouble(0.000, 0.000, 0.000),

    HAD0constDR = cms.vdouble(0.100, 0.100, 0.150),
    HAD0floatDR = cms.vdouble(0.000, 0.000, 0.000),
    HAD0capDR =   cms.vdouble(0.000, 0.000, 0.000),

    HADCHconstDR = cms.vdouble(0.002, 0.002, 0.003),
    HADCHfloatDR = cms.vdouble(0.010, 0.010, 0.015),
    HADCHcapDR =   cms.vdouble(0.050, 0.050, 0.070),

    ELEconstDR = cms.vdouble(0.002, 0.002, 0.003),
    ELEfloatDR = cms.vdouble(0.010, 0.010, 0.015),
    ELEcapDR =   cms.vdouble(0.050, 0.050, 0.070),

    MUconstDR = cms.vdouble(0.002, 0.002, 0.003),
    MUfloatDR = cms.vdouble(0.010, 0.010, 0.015),
    MUcapDR =   cms.vdouble(0.050, 0.050, 0.070),

    maxReFit = cms.uint32(50),

    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("GenSimonJets"),

    verbose = cms.int32(1)
)
