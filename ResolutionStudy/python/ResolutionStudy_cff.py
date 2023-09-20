import FWCore.ParameterSet.Config as cms

from SRothman.Matching.Matching_cff import *
from SRothman.CustomJets.CustomJets_cff import *
from SRothman.Matching.RuleMatchProducer_cfi import *
from SRothman.ResolutionStudy.ResolutionStudyTable_cfi import *

def setupResolutionStudy(process, verbose=0):
    process = addCustomJets(process, verbose=verbose, table=False)
    process.SimonJets.doEventSelection = False
    process.GenSimonJets.doEventSelection = False

    process = addGenMatching(process, verbose=verbose,
                             name='DefaultMatch')
    process = addGenMatching(process, verbose=verbose,
                             name='NaiveMatch',
                             prefitters = ["Best","Best","Best"],
                             recoverLostTracks = False,
                             filters = ['Charge', 'Charge', 'Charge'],
                             refiner = 'None',
                             dropGenFilter = 'NONE',
                             dropRecoFilter = 'NONE')
    process = addGenMatching(process, verbose=verbose,
                             name='NewParamsMatch',
                             PUpenalties = [8.0, 4.0, 8.0],
                             PUexps = [8.0, 8.0, 8.0],
                             PUpt0s = [1.0, 3.0, 0.5],

                             EMstochastic = cms.vdouble(0.07, 0.10, 0.08),
                             EMnoise = cms.vdouble(0.0, 0.0, 0.0),
                             EMconstant = cms.vdouble(0.02, 0.04, 0.03),
                             ECALgranularityEta = cms.vdouble(0.0027, 0.0030, 0.0035),
                             ECALgranularityPhi = cms.vdouble(0.0027, 0.0030, 0.0035),

                             HADstochastic = cms.vdouble(1.00, 1.00, 1.00),
                             HADconstant = cms.vdouble(0.12, 0.24, 0.18),
                             HCALgranularityEta = cms.vdouble(0.04, 0.05, 0.08),
                             HCALgranularityPhi = cms.vdouble(0.04, 0.05, 0.08),

                             CHlinear = cms.vdouble(0.00014, 0.00014, 0.0017),
                             CHconstant = cms.vdouble(0.011, 0.017, 0.019),
                             CHMSeta = cms.vdouble(0.0017, 0.0019, 0.0028),
                             CHangularEta = cms.vdouble(0.00029, 0.00026, 0.0034),
                             CHMSphi = cms.vdouble(0.0018, 0.0021, 0.0031),
                             CHangularPhi = cms.vdouble(0.00021, 0.00025, 0.0032),

                             dropGenFilter = 'ALL',
                             dropRecoFilter = 'ALL')
    process = addGenMatching(process, verbose=verbose,
                             name='NoRecoDropMatch',
                             PUpenalties = [8.0, 4.0, 8.0],
                             PUexps = [8.0, 8.0, 8.0],
                             PUpt0s = [1.0, 3.0, 0.5],

                             EMstochastic = cms.vdouble(0.07, 0.10, 0.08),
                             EMnoise = cms.vdouble(0.0, 0.0, 0.0),
                             EMconstant = cms.vdouble(0.02, 0.04, 0.03),
                             ECALgranularityEta = cms.vdouble(0.0027, 0.0030, 0.0035),
                             ECALgranularityPhi = cms.vdouble(0.0027, 0.0030, 0.0035),

                             HADstochastic = cms.vdouble(1.00, 1.00, 1.00),
                             HADconstant = cms.vdouble(0.12, 0.24, 0.18),
                             HCALgranularityEta = cms.vdouble(0.04, 0.05, 0.08),
                             HCALgranularityPhi = cms.vdouble(0.04, 0.05, 0.08),

                             CHlinear = cms.vdouble(0.00014, 0.00014, 0.0017),
                             CHconstant = cms.vdouble(0.011, 0.017, 0.019),
                             CHMSeta = cms.vdouble(0.0017, 0.0019, 0.0028),
                             CHangularEta = cms.vdouble(0.00029, 0.00026, 0.0034),
                             CHMSphi = cms.vdouble(0.0018, 0.0021, 0.0031),
                             CHangularPhi = cms.vdouble(0.00021, 0.00025, 0.0032),

                             dropGenFilter = 'ALL',
                             dropRecoFilter = 'NONE')
    process = addGenMatching(process, verbose=verbose,
                             name='NoGenDropMatch',
                             PUpenalties = [8.0, 4.0, 8.0],
                             PUexps = [8.0, 8.0, 8.0],
                             PUpt0s = [1.0, 3.0, 0.5],

                             EMstochastic = cms.vdouble(0.07, 0.10, 0.08),
                             EMnoise = cms.vdouble(0.0, 0.0, 0.0),
                             EMconstant = cms.vdouble(0.02, 0.04, 0.03),
                             ECALgranularityEta = cms.vdouble(0.0027, 0.0030, 0.0035),
                             ECALgranularityPhi = cms.vdouble(0.0027, 0.0030, 0.0035),

                             HADstochastic = cms.vdouble(1.00, 1.00, 1.00),
                             HADconstant = cms.vdouble(0.12, 0.24, 0.18),
                             HCALgranularityEta = cms.vdouble(0.04, 0.05, 0.08),
                             HCALgranularityPhi = cms.vdouble(0.04, 0.05, 0.08),

                             CHlinear = cms.vdouble(0.00014, 0.00014, 0.0017),
                             CHconstant = cms.vdouble(0.011, 0.017, 0.019),
                             CHMSeta = cms.vdouble(0.0017, 0.0019, 0.0028),
                             CHangularEta = cms.vdouble(0.00029, 0.00026, 0.0034),
                             CHMSphi = cms.vdouble(0.0018, 0.0021, 0.0031),
                             CHangularPhi = cms.vdouble(0.00021, 0.00025, 0.0032),

                             dropGenFilter = 'NONE',
                             dropRecoFilter = 'ALL')
    process = addGenMatching(process, verbose=verbose,
                             name='NoDropMatch',
                             PUpenalties = [8.0, 4.0, 8.0],
                             PUexps = [8.0, 8.0, 8.0],
                             PUpt0s = [1.0, 3.0, 0.5],

                             EMstochastic = cms.vdouble(0.07, 0.10, 0.08),
                             EMnoise = cms.vdouble(0.0, 0.0, 0.0),
                             EMconstant = cms.vdouble(0.02, 0.04, 0.03),
                             ECALgranularityEta = cms.vdouble(0.0027, 0.0030, 0.0035),
                             ECALgranularityPhi = cms.vdouble(0.0027, 0.0030, 0.0035),

                             HADstochastic = cms.vdouble(1.00, 1.00, 1.00),
                             HADconstant = cms.vdouble(0.12, 0.24, 0.18),
                             HCALgranularityEta = cms.vdouble(0.04, 0.05, 0.08),
                             HCALgranularityPhi = cms.vdouble(0.04, 0.05, 0.08),

                             CHlinear = cms.vdouble(0.00014, 0.00014, 0.0017),
                             CHconstant = cms.vdouble(0.011, 0.017, 0.019),
                             CHMSeta = cms.vdouble(0.0017, 0.0019, 0.0028),
                             CHangularEta = cms.vdouble(0.00029, 0.00026, 0.0034),
                             CHMSphi = cms.vdouble(0.0018, 0.0021, 0.0031),
                             CHangularPhi = cms.vdouble(0.00021, 0.00025, 0.0032),

                             dropGenFilter = 'NONE',
                             dropRecoFilter = 'NONE')
    return process
