import FWCore.ParameterSet.Config as cms

GenMatchProducer = cms.EDProducer("GenMatchProducer",
    jetMatchingDR = cms.double(0.4),

    clipval = cms.double(0.05),

    spatialLoss = cms.int32(0),
    filter = cms.vint32([2, 4]),
    uncertainty = cms.vint32([1, 1]),

    cutoff = cms.double(2.0),

    softPt = cms.double(1.0),
    hardPt = cms.double(110),

    EMstochastic = cms.vdouble(0.021, 0.060, 0.042),
    EMnoise = cms.vdouble(0.094, 0.1, 0.2),
    EMconstant = cms.vdouble(0.005, 0.015, 0.01),
    ECALgranularity = cms.vdouble(0.0175, 0.025, 0.05),
    ECALEtaBoundaries = cms.vdouble(0.0, 1.1, 1.6, 3.0),

    HADstochastic = cms.vdouble(0.45, 0.90, 0.60),
    HADconstant = cms.vdouble(0.05, 0.10, 0.07),
    HCALgranularity = cms.vdouble(0.087, 0.087, 0.175),
    HCALEtaBoundaries = cms.vdouble(0.0, 1.3, 1.74, 3.0),

    #NB tracker angular resolutions are pretty ad-hoc
    CHlinear = cms.vdouble(0.00025, 0.00050, 0.00075),
    CHconstant = cms.vdouble(0.015, 0.030, 0.045),
    CHMS = cms.vdouble(0.002, 0.002, 0.004),
    CHangular = cms.vdouble(0.0005, 0.0010, 0.0015),
    trkEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 2.5),

    maxReFit = cms.uint32(50),
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("SimonGenJets"),
    recoParts=cms.InputTag(""),
    genParts=cms.InputTag(""),
    doLargerCollections=cms.bool(False),
    verbose = cms.int32(1)
)
