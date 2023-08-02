import FWCore.ParameterSet.Config as cms

GenMatchProducer = cms.EDProducer("GenMatchProducer",
    jetMatchingDR = cms.double(0.2),

    clipval = cms.double(0.05),

    spatialLoss = cms.int32(0),
    #I wonder whether the PU parameters should be different for different eta regions
    PUpt0s = cms.vdouble(1.0, 1.0, 1.0),
    PUexps = cms.vdouble(8.0, 8.0, 8.0), #not sure about this value
    PUpenalties = cms.vdouble(4.0, 4.0, 4.0), #makes sense to homogonize with the cutoff values
                                  
    uncertainty = cms.string("Standard"),

    filters = cms.vstring("Charge", "Charge", "Charge"),
    cutoffs = cms.vdouble(4.0, 4.0, 4.0),
    prefitters = cms.vstring("Float", "Float", "Best"),
    refiner = cms.string("OneGenOneRecoPerType"),
    dropGenFilter = cms.string("CHARGED"),
    dropRecoFilter = cms.string("ALL"),

    recoverLostTracks = cms.bool(True),

    #things seem weird in endcaps for pt resolution
    #for the moment just using barrel numbers across the board
    #also for the moment treat noise term = 0
    EMstochastic = cms.vdouble(6.51e-2, 6.51e-2, 6.51e-2), 
    EMnoise = cms.vdouble(0.0, 0.0, 0.0),
    EMconstant = cms.vdouble(2.01e-2, 2.01e-2, 2.01e-2),
    ECALgranularityEta = cms.vdouble(3.71e-3, 4.77e-3, 7.03e-3),
    ECALgranularityPhi = cms.vdouble(4.70e-3, 6.17e-3, 7.72e-3),
    ECALEtaBoundaries = cms.vdouble(0.0, 1.1, 1.6, 3.0),

    #the fits for pt resolution are pretty bad, but okay
    HADstochastic = cms.vdouble(57.8e-2, 57.8e-2, 57.8e-2),
    HADconstant = cms.vdouble(19.4e-2, 19.4e-2, 19.4e-2),
    HCALgranularityEta = cms.vdouble(4.41e-2, 5.52e-2, 9.03e-2),
    HCALgranularityPhi = cms.vdouble(4.70e-2, 5.29e-2, 8.50e-2),
    HCALEtaBoundaries = cms.vdouble(0.0, 1.3, 1.74, 3.0),

    #NB tracker angular resolutions are pretty ad-hoc
    CHlinear = cms.vdouble(2.92e-4, 5.07e-4, 5.49e-4),
    CHconstant = cms.vdouble(1.07e-2, 2.09e-2, 3.40e-2),
    CHMSeta = cms.vdouble(1.49e-3, 1.74e-3, 2.59e-3),
    CHMSphi = cms.vdouble(1.56e-3, 1.88e-3, 2.83e-3),
    CHangularEta = cms.vdouble(2.96e-4, 2.65e-4, 3.38e-4),
    CHangularPhi = cms.vdouble(1.73e-4, 2.14e-4, 2.68e-4),
    trkEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 2.5),

    maxReFit = cms.uint32(50),

    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("SimonGenJets"),
    recoParts=cms.InputTag(""),
    genParts=cms.InputTag(""),
    doLargerCollections=cms.bool(False),

    verbose = cms.int32(1)
)
