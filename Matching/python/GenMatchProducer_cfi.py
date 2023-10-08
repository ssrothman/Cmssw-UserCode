import FWCore.ParameterSet.Config as cms

# goes [EM0, HAD0, HADCH, ELE, MU]

GenMatchProducer = cms.EDProducer("GenMatchProducer",
    jetMatchingDR = cms.double(0.2),

    clipval = cms.double(0.01),

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
    ECALgranularityEta = cms.vdouble(0.01, 0.01, 0.01),
    ECALgranularityPhi = cms.vdouble(0.01, 0.01, 0.01),
    ECALEtaBoundaries = cms.vdouble(0.0, 1.1, 1.6, 3.0),

    HADstochastic = cms.vdouble(1.63, 3.90, 6.44),
    HADconstant = cms.vdouble(0.21, 0.14, 0.10),
    HCALgranularityEta = cms.vdouble(0.01, 0.01, 0.01),
    HCALgranularityPhi = cms.vdouble(0.01, 0.01, 0.01),
    HCALEtaBoundaries = cms.vdouble(0.0, 1.3, 1.74, 3.0),

    CHlinear = cms.vdouble(0.000069, 0.000072, 0.000072),
    CHconstant = cms.vdouble(0.0076, 0.014, 0.018),
    CHMSeta = cms.vdouble(0.0000, 0.0000, 0.0000),
    CHMSphi = cms.vdouble(0.0000, 0.0000, 0.0000),
    CHangularEta = cms.vdouble(0.001, 0.001, 0.001),
    CHangularPhi = cms.vdouble(0.001, 0.001, 0.001),
    trkEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    maxReFit = cms.uint32(50),

    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("SimonGenJets"),

    verbose = cms.int32(1)
)
