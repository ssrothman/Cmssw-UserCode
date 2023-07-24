import FWCore.ParameterSet.Config as cms

GenMatchProducer = cms.EDProducer("GenMatchProducer",
    jetMatchingDR = cms.double(0.2),

    clipval = cms.double(0.05),

    spatialLoss = cms.int32(0),
    filter = cms.int32(2),
    uncertainty = cms.int32(1),
    prefitters = cms.vint32([4, 4, 3]),
    refiner = cms.int32(2),

    PUexp = cms.double(4),
    PUpenalty = cms.double(2),

    recoverLostTracks = cms.bool(True),

    greedyDropMatches = cms.bool(False),
    greedyDropGen = cms.bool(False),
    greedyDropReco = cms.bool(False),

    cutoff = cms.double(3.0),

    softPt = cms.double(1.0),
    hardPt = cms.double(110),

    EMstochastic = cms.vdouble(0.021, 0.030, 0.020),
    EMnoise = cms.vdouble(0.094, 0.1, 0.1),
    EMconstant = cms.vdouble(0.005, 0.015, 0.01),
    ECALgranularity = cms.vdouble(0.0175, 0.025, 0.04),
    ECALEtaBoundaries = cms.vdouble(0.0, 1.1, 1.6, 3.0),

    HADstochastic = cms.vdouble(0.45, 0.45, 0.45),
    HADconstant = cms.vdouble(0.05, 0.10, 0.07),
    HCALgranularity = cms.vdouble(0.087, 0.087, 0.175),
    HCALEtaBoundaries = cms.vdouble(0.0, 1.3, 1.74, 3.0),

    #NB tracker angular resolutions are pretty ad-hoc
    CHlinear = cms.vdouble(0.00025, 0.00050, 0.00075),
    CHconstant = cms.vdouble(0.015, 0.030, 0.045),
    CHMS = cms.vdouble(0.00185, 0.00215, 0.0035),
    CHangular = cms.vdouble(0.00053, 0.00053, 0.0007),
    trkEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 2.5),

    maxReFit = cms.uint32(50),
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("SimonGenJets"),
    recoParts=cms.InputTag(""),
    genParts=cms.InputTag(""),
    doLargerCollections=cms.bool(False),
    verbose = cms.int32(1)
)
