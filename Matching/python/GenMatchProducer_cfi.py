import FWCore.ParameterSet.Config as cms

# goes [EM0, HAD0, HADCH, ELE, MU]

GenMatchProducer = cms.EDProducer("GenMatchProducer",
    jetMatchingDR = cms.double(0.4),

    #clip the energy sharing fractions less than clipval to zero
    #and those greater than 1-clipval to 1
    clipval = cms.double(0.01),

    #there are in principle two options for the spatial loss function, but in practice you always want TYPE1 = value 0
    spatialLoss = cms.int32(0),

    #there are some PU terms in the loss function
    #which penalize unmatched particles
    #the form of the penalty is (PUpenalty + (pT/PUpt0)^PUexp)
    #this is only relevant if the particle dropping is enabled (see below)
    #the lists have length 5, one for each particle type
    #in order [EM0, HAD0, HADCH, ELE, MU]
    PUpt0s = cms.vdouble(1.0, 1.0, 
                         1.0, 1.0, 1.0),
    PUexps = cms.vdouble(4.0, 4.0, 
                         4.0, 4.0, 4.0),
    PUpenalties = cms.vdouble(2.0e12, 2.0e12, 
                              2.0e12, 2.0e12, 2.0e12),

    #uncertainty model to use
    #there are a few options in ParticleUncertainty.cc
    #"Standard" is a cms-like model:
    #    photons have deltaE/E = A/sqrt(E) (+) B
    #                 delta eta = C
    #                 delta phi = C
    #    hadrons have deltaE/E = A/sqrt(E) (+) B
    #                 delta eta = C
    #                 delta phi = C
    #    tracks have delta pT/pT = A*pT (+) B
    #                 delta eta = C (+) D/pT
    #                 delta phi = C (+) D/pT
    #    parameters are below
    uncertainty = cms.string("Standard"),

    #flavor filters for which particle species can match with which
    #the lists have length 5, one for each RECO particle type
    #in order [EM0, HAD0, HADCH, ELE, MU]
    #there are a million options in MatchingFilter.cc
    #also, can have different filters in two different pT regimes
    #with the crossover set by the filterthresholds
    softflavorfilters = cms.vstring(
        "AnyPhoton", "AnyNeutralHadron", 
        "AnyCharged", "AnyCharged", "AnyCharged"),
    hardflavorfilters = cms.vstring(
        "AnyNeutral", "AnyNeutralHadron", 
        "AnyCharged", "AnyCharged", "AnyCharged"),
    filterthresholds = cms.vdouble(3.0, 0.0, 0.0, 0.0, 0.0),

    #filters for which particle charges can match with which
    #the lists have length 5, one for each RECO particle type
    #in order [EM0, HAD0, HADCH, ELE, MU]
    #options in MatchingFilter.cc
    chargefilters = cms.vstring(
        'ChargeSign', 'ChargeSign',
        'ChargeSign', 'ChargeSign', 'ChargeSign'),

    #filters in delta R for which particles can match
    #there are two types of dR filters, "Fixed" and "Tracking"
    #"Fixed" is a fixed dR cut
    #"Tracking" is a dR cut that scales with the pT of the particle
    #according to track uncertainty
    #the parameters that govern these are below
    dRfilters = cms.vstring(
        "Fixed", "Fixed", 
        'Tracking', 'Tracking', 'Tracking'),

    #prefitters the preimilary matches that are allowed
    #"Float" allows multiple matches, which can then be floated in the fit
    #"Best" uses a chi-squared likelihood to 
    #       only select the best available match
    #
    #these are in terms of RECO particles. 
    #ie if you say "Best", you are requiring that each RECO particle have 
    #exactly one match, but not necessarily that each GEN particle have only
    #one match
    #details are in prefit.cc
    prefitters = cms.vstring(
        "Float", "Float", 
        "Best", "Best", "Best"),

    #The refiner refines the prefit based on how many matches the gen particles have
    #There are a few options in refinePrefit.cc
    #"OneGenOneReco" requires that each GEN particle have exactly one match
    #in each subdetector. ie at most one track, one photon, one hadron
    refiner = cms.string("OneGenOneRecoPerType"),

    #allow matching process to test if fit improves by dropping particles
    #the filters restrict which particles can be dropped
    #options are in particleFilter.cc
    dropGenFilter = cms.string("NONE"),
    dropRecoFilter = cms.string("NONE"),

    #whether to attempt track recovery
    recoverLostTracks = cms.bool(True),
    #whether to propagate lost tracks in the B field to the front face 
    #of the appropriate calorimeter
    #NB this only works inside CMSSW, as I am using some CMSSW functions
    propagateLostTracks = cms.bool(True),
    #min pT for charged hadrons to be considered for recovery
    HADCHrecoverThresholds = cms.vdouble(5.0, 5.0, 5.0),
    #min pT for electrons to be considered for recovery
    ELErecoverThresholds = cms.vdouble(5.0, 5.0, 5.0),

    #whether to try to recover missing HAD0s in the ECAL
    recoverLostHAD0 = cms.bool(False),
    #min pT for HAD0s to be considered for recovery
    HAD0recoverThresholds = cms.vdouble(0.0, 0.0, 0.0),

    #parameters for ECAL uncertainty model
    EMstochastic = cms.vdouble(0.17, 0.18, 0.80),
    EMconstant = cms.vdouble(0.0074, 0.0253, 0.0253),
    ECALgranularityEta = cms.vdouble(0.05, 0.05, 0.07),
    ECALgranularityPhi = cms.vdouble(0.05, 0.05, 0.07),
    ECALEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    #parameters for HCAL uncertainty model
    HADstochastic = cms.vdouble(1.63, 3.90, 6.44),
    HADconstant = cms.vdouble(0.21, 0.14, 0.10),
    HCALgranularityEta = cms.vdouble(0.10, 0.10, 0.15),
    HCALgranularityPhi = cms.vdouble(0.10, 0.10, 0.15),
    HCALEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    #parameters for track uncertainty model
    CHlinear = cms.vdouble(0.000069, 0.000072, 0.000072),
    CHconstant = cms.vdouble(0.0076, 0.014, 0.018),
    CHMSeta = cms.vdouble(0.0000, 0.0000, 0.0000),
    CHMSphi = cms.vdouble(0.0000, 0.0000, 0.0000),
    CHangularEta = cms.vdouble(0.001, 0.001, 0.001),
    CHangularPhi = cms.vdouble(0.001, 0.001, 0.001),
    trkEtaBoundaries = cms.vdouble(0.0, 0.9, 1.4, 3.0),

    #min pT thresholds for matching
    EM0thresholds =   cms.vdouble(0.0, 0.0, 0.0),
    HAD0thresholds =  cms.vdouble(0.0, 0.0, 0.0),
    HADCHthresholds = cms.vdouble(0.0, 0.0, 0.0),
    ELEthresholds =   cms.vdouble(0.0, 0.0, 0.0),
    MUthresholds =    cms.vdouble(0.0, 0.0, 0.0),

    #parameters for dR filters
    #for "fixed" filter, the threshold is constDR
    #for "tracking" filter, the threshold looks like
    #      constDR (+) floatDR/pT
    #      but is capped at capDR, to avoid the divergence at low pT

    #photon dR windows
    EM0constDR = cms.vdouble(0.050, 0.050, 0.050),
    EM0floatDR = cms.vdouble(0.000, 0.000, 0.000),
    EM0capDR =   cms.vdouble(0.000, 0.000, 0.000),

    #hadron dR windows
    HAD0constDR = cms.vdouble(0.100, 0.100, 0.150),
    HAD0floatDR = cms.vdouble(0.000, 0.000, 0.000),
    HAD0capDR =   cms.vdouble(0.000, 0.000, 0.000),

    #charged hadron dR windows
    HADCHconstDR = cms.vdouble(0.002, 0.002, 0.003),
    HADCHfloatDR = cms.vdouble(0.010, 0.010, 0.015),
    HADCHcapDR =   cms.vdouble(0.050, 0.050, 0.070),

    #electron dR windows
    ELEconstDR = cms.vdouble(0.002, 0.002, 0.003),
    ELEfloatDR = cms.vdouble(0.010, 0.010, 0.015),
    ELEcapDR =   cms.vdouble(0.050, 0.050, 0.070),

    #muon dR windows
    MUconstDR = cms.vdouble(0.002, 0.002, 0.003),
    MUfloatDR = cms.vdouble(0.010, 0.010, 0.015),
    MUcapDR =   cms.vdouble(0.050, 0.050, 0.070),

    #the fitting repeats with the clipping steps (see clipval above)
    #until convergence, or maxReFit
    maxReFit = cms.uint32(50),

    #input collections
    reco = cms.InputTag("SimonJets"),
    gen = cms.InputTag("GenSimonJets"),

    #verbosity parameter
    #0 is silent, values larger than 0 print more and more
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
