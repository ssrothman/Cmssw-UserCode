import FWCore.ParameterSet.Config as cms

# goes [EM0, HAD0, HADCH, ELE, MU]

GenMatchProducer = cms.EDProducer("GenMatchProducer",
    jetMatchingDR = cms.double(0.2),

    clipval = cms.double(0.05),

    spatialLoss = cms.int32(0),

    cutoffs = cms.vdouble(4.0, 4.0, 
                          4.0, 4.0, 4.0),
    filters = cms.vstring("AnyPhoton", "AnyNeutralHadron", 
                          "AnyChargedHadron", "AnyElectron", "AnyMuon"),
    prefitters = cms.vstring("Float", "Float", 
                             "Best", "Best", "Best"),

    refiner = cms.string("OneGenOneRecoPerType"),
    dropGenFilter = cms.string("NONE"),
    dropRecoFilter = cms.string("NONE"),
    recoverLostTracks = cms.bool(False),

    PUpt0s = cms.vdouble(1.0, 1.0, 
                         1.0, 1.0, 1.0),
    PUexps = cms.vdouble(4.0, 4.0, 
                         4.0, 4.0, 4.0),
    PUpenalties = cms.vdouble(2.0e12, 2.0e12, 
                              2.0e12, 2.0e12, 2.0e12),
                                  
    uncertainty = cms.string("Standard"),

    EMstochastic = cms.vdouble(0.17, 0.18, 0.80),
    EMconstant = cms.vdouble(0.0074, 0.0253, 0.0253),
    ECALgranularityEta = cms.vdouble(0.0017, 0.0035, 0.0045),
    ECALgranularityPhi = cms.vdouble(0.0018, 0.0050, 0.0055),
    ECALEtaBoundaries = cms.vdouble(0.0, 1.1, 1.6, 3.0),

    HADstochastic = cms.vdouble(1.63, 3.90, 6.44),
    HADconstant = cms.vdouble(0.21, 0.14, 0.10),
    HCALgranularityEta = cms.vdouble(0.016, 0.030, 0.023),
    HCALgranularityPhi = cms.vdouble(0.018, 0.024, 0.020),
    HCALEtaBoundaries = cms.vdouble(0.0, 1.3, 1.74, 3.0),

    CHlinear = cms.vdouble(0.000069, 0.000072, 0.000072),
    CHconstant = cms.vdouble(0.0076, 0.014, 0.018),
    CHMSeta = cms.vdouble(0.0015, 0.0017, 0.0022),
    CHMSphi = cms.vdouble(0.0016, 0.0020, 0.0026),
    CHangularEta = cms.vdouble(0.00024, 0.00026, 0.00024),
    CHangularPhi = cms.vdouble(0.00017, 0.00020, 0.00019),
    trkEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    maxReFit = cms.uint32(50),

    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("SimonGenJets"),

    verbose = cms.int32(1)
)
