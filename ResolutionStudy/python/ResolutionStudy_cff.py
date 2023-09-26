import FWCore.ParameterSet.Config as cms

from SRothman.Matching.Matching_cff import *
from SRothman.CustomJets.CustomJets_cff import *
from SRothman.Matching.RuleMatchProducer_cfi import *
from SRothman.ResolutionStudy.ResolutionStudyTable_cfi import *

def setupResolutionStudy(process, verbose=0):
    process = addCustomJets(process, verbose=verbose, table=False)
    process.SimonJets.doEventSelection = False
    process.GenSimonJets.doEventSelection = False
    process.FullEventJets.doEventSelection = False
    process.GenFullEventJets.doEventSelection = False

    process = addGenMatching(process, verbose=verbose,
                             name='NaiveMatch',

                             cutoffs = [32.0, 32.0, 32.0],
                             filters = ['Charge', 'Charge', 'Charge'],
                             prefitters = cms.vstring("Best", "Best", "Best"),
                             refiner = 'OneGenOneReco',
                             dropGenFilter = 'NONE',
                             dropRecoFilter = 'NONE',
                             recoverLostTracks = False,

                             PUpenalties = [32.0, 32.0, 32.0],
                             PUexps = [8.0, 8.0, 8.0],
                             PUpt0s = [1.0, 3.0, 0.5],

                             EMstochastic = cms.vdouble(0.07, 0.10, 0.08),
                             EMnoise = cms.vdouble(0.0, 0.0, 0.0),
                             EMconstant = cms.vdouble(0.02, 0.04, 0.03),
                             ECALgranularityEta = cms.vdouble(0.0050, 0.0070, 0.0100),
                             ECALgranularityPhi = cms.vdouble(0.0050, 0.0070, 0.0100),

                             HADstochastic = cms.vdouble(1.00, 1.00, 1.00),
                             HADconstant = cms.vdouble(0.12, 0.24, 0.18),
                             HCALgranularityEta = cms.vdouble(0.05, 0.07, 0.10),
                             HCALgranularityPhi = cms.vdouble(0.05, 0.07, 0.10),

                             CHlinear = cms.vdouble(0.00014, 0.00014, 0.0017),
                             CHconstant = cms.vdouble(0.011, 0.017, 0.019),
                             CHMSeta = cms.vdouble(0.0017, 0.0019, 0.0028),
                             CHangularEta = cms.vdouble(0.00030, 0.00035, 0.0042),
                             CHMSphi = cms.vdouble(0.0018, 0.0021, 0.0031),
                             CHangularPhi = cms.vdouble(0.00030, 0.00035, 0.0042))

    process = addGenMatching(process, verbose=verbose,
                             name='OneToOneFloatMatch',

                             cutoffs = [32.0, 32.0, 32.0],
                             filters = ['Charge', 'Charge', 'Charge'],
                             prefitters = cms.vstring("Float", "Float", "Best"),
                             refiner = 'OneGenOneReco',
                             dropGenFilter = 'NONE',
                             dropRecoFilter = 'NONE',
                             recoverLostTracks = False,

                             PUpenalties = [32.0, 32.0, 32.0],
                             PUexps = [8.0, 8.0, 8.0],
                             PUpt0s = [1.0, 3.0, 0.5],

                             EMstochastic = cms.vdouble(0.07, 0.10, 0.08),
                             EMnoise = cms.vdouble(0.0, 0.0, 0.0),
                             EMconstant = cms.vdouble(0.02, 0.04, 0.03),
                             ECALgranularityEta = cms.vdouble(0.0050, 0.0070, 0.0100),
                             ECALgranularityPhi = cms.vdouble(0.0050, 0.0070, 0.0100),

                             HADstochastic = cms.vdouble(1.00, 1.00, 1.00),
                             HADconstant = cms.vdouble(0.12, 0.24, 0.18),
                             HCALgranularityEta = cms.vdouble(0.05, 0.07, 0.10),
                             HCALgranularityPhi = cms.vdouble(0.05, 0.07, 0.10),

                             CHlinear = cms.vdouble(0.00014, 0.00014, 0.0017),
                             CHconstant = cms.vdouble(0.011, 0.017, 0.019),
                             CHMSeta = cms.vdouble(0.0017, 0.0019, 0.0028),
                             CHangularEta = cms.vdouble(0.00030, 0.00035, 0.0042),
                             CHMSphi = cms.vdouble(0.0018, 0.0021, 0.0031),
                             CHangularPhi = cms.vdouble(0.00030, 0.00035, 0.0042))

    process = addGenMatching(process, verbose=verbose,
                             name='OneToTwoFloatMatch',

                             cutoffs = [32.0, 32.0, 32.0],
                             filters = ['Charge', 'Charge', 'Charge'],
                             prefitters = cms.vstring("Float", "Float", "Best"),
                             refiner = 'OneGenOneRecoPerType',
                             dropGenFilter = 'NONE',
                             dropRecoFilter = 'NONE',
                             recoverLostTracks = False,

                             PUpenalties = [32.0, 32.0, 32.0],
                             PUexps = [8.0, 8.0, 8.0],
                             PUpt0s = [1.0, 3.0, 0.5],

                             EMstochastic = cms.vdouble(0.07, 0.10, 0.08),
                             EMnoise = cms.vdouble(0.0, 0.0, 0.0),
                             EMconstant = cms.vdouble(0.02, 0.04, 0.03),
                             ECALgranularityEta = cms.vdouble(0.0050, 0.0070, 0.0100),
                             ECALgranularityPhi = cms.vdouble(0.0050, 0.0070, 0.0100),

                             HADstochastic = cms.vdouble(1.00, 1.00, 1.00),
                             HADconstant = cms.vdouble(0.12, 0.24, 0.18),
                             HCALgranularityEta = cms.vdouble(0.05, 0.07, 0.10),
                             HCALgranularityPhi = cms.vdouble(0.05, 0.07, 0.10),

                             CHlinear = cms.vdouble(0.00014, 0.00014, 0.0017),
                             CHconstant = cms.vdouble(0.011, 0.017, 0.019),
                             CHMSeta = cms.vdouble(0.0017, 0.0019, 0.0028),
                             CHangularEta = cms.vdouble(0.00030, 0.00035, 0.0042),
                             CHMSphi = cms.vdouble(0.0018, 0.0021, 0.0031),
                             CHangularPhi = cms.vdouble(0.00030, 0.00035, 0.0042))

    process = addGenMatching(process, verbose=verbose,
                             name='OneToTwoDropGenMatch',

                             cutoffs = [32.0, 32.0, 32.0],
                             filters = ['Charge', 'Charge', 'Charge'],
                             prefitters = cms.vstring("Float", "Float", "Best"),
                             refiner = 'OneGenOneRecoPerType',
                             dropGenFilter = 'ALL',
                             dropRecoFilter = 'NONE',
                             recoverLostTracks = False,

                             PUpenalties = [32.0, 32.0, 32.0],
                             PUexps = [8.0, 8.0, 8.0],
                             PUpt0s = [1.0, 3.0, 0.5],

                             EMstochastic = cms.vdouble(0.07, 0.10, 0.08),
                             EMnoise = cms.vdouble(0.0, 0.0, 0.0),
                             EMconstant = cms.vdouble(0.02, 0.04, 0.03),
                             ECALgranularityEta = cms.vdouble(0.0050, 0.0070, 0.0100),
                             ECALgranularityPhi = cms.vdouble(0.0050, 0.0070, 0.0100),

                             HADstochastic = cms.vdouble(1.00, 1.00, 1.00),
                             HADconstant = cms.vdouble(0.12, 0.24, 0.18),
                             HCALgranularityEta = cms.vdouble(0.05, 0.07, 0.10),
                             HCALgranularityPhi = cms.vdouble(0.05, 0.07, 0.10),

                             CHlinear = cms.vdouble(0.00014, 0.00014, 0.0017),
                             CHconstant = cms.vdouble(0.011, 0.017, 0.019),
                             CHMSeta = cms.vdouble(0.0017, 0.0019, 0.0028),
                             CHangularEta = cms.vdouble(0.00030, 0.00035, 0.0042),
                             CHMSphi = cms.vdouble(0.0018, 0.0021, 0.0031),
                             CHangularPhi = cms.vdouble(0.00030, 0.00035, 0.0042))

    process = addGenMatching(process, verbose=verbose,
                             name='OneToTwoDropGenRecoverMatch',

                             cutoffs = [32.0, 32.0, 32.0],
                             filters = ['Charge', 'Charge', 'Charge'],
                             prefitters = cms.vstring("Float", "Float", "Best"),
                             refiner = 'OneGenOneRecoPerType',
                             dropGenFilter = 'ALL',
                             dropRecoFilter = 'NONE',
                             recoverLostTracks = True,

                             PUpenalties = [32.0, 32.0, 32.0],
                             PUexps = [8.0, 8.0, 8.0],
                             PUpt0s = [1.0, 3.0, 0.5],

                             EMstochastic = cms.vdouble(0.07, 0.10, 0.08),
                             EMnoise = cms.vdouble(0.0, 0.0, 0.0),
                             EMconstant = cms.vdouble(0.02, 0.04, 0.03),
                             ECALgranularityEta = cms.vdouble(0.0050, 0.0070, 0.0100),
                             ECALgranularityPhi = cms.vdouble(0.0050, 0.0070, 0.0100),

                             HADstochastic = cms.vdouble(1.00, 1.00, 1.00),
                             HADconstant = cms.vdouble(0.12, 0.24, 0.18),
                             HCALgranularityEta = cms.vdouble(0.05, 0.07, 0.10),
                             HCALgranularityPhi = cms.vdouble(0.05, 0.07, 0.10),

                             CHlinear = cms.vdouble(0.00014, 0.00014, 0.0017),
                             CHconstant = cms.vdouble(0.011, 0.017, 0.019),
                             CHMSeta = cms.vdouble(0.0017, 0.0019, 0.0028),
                             CHangularEta = cms.vdouble(0.00030, 0.00035, 0.0042),
                             CHMSphi = cms.vdouble(0.0018, 0.0021, 0.0031),
                             CHangularPhi = cms.vdouble(0.00030, 0.00035, 0.0042))

    process = addGenMatching(process, verbose=verbose,
                             name='FloatMatch',

                             cutoffs = [32.0, 32.0, 32.0],
                             filters = ['Charge', 'Charge', 'Charge'],
                             prefitters = cms.vstring("Float", "Float", "Best"),
                             refiner = 'None',
                             dropGenFilter = 'NONE',
                             dropRecoFilter = 'NONE',
                             recoverLostTracks = False,

                             PUpenalties = [32.0, 32.0, 32.0],
                             PUexps = [8.0, 8.0, 8.0],
                             PUpt0s = [1.0, 3.0, 0.5],

                             EMstochastic = cms.vdouble(0.07, 0.10, 0.08),
                             EMnoise = cms.vdouble(0.0, 0.0, 0.0),
                             EMconstant = cms.vdouble(0.02, 0.04, 0.03),
                             ECALgranularityEta = cms.vdouble(0.0050, 0.0070, 0.0100),
                             ECALgranularityPhi = cms.vdouble(0.0050, 0.0070, 0.0100),

                             HADstochastic = cms.vdouble(1.00, 1.00, 1.00),
                             HADconstant = cms.vdouble(0.12, 0.24, 0.18),
                             HCALgranularityEta = cms.vdouble(0.05, 0.07, 0.10),
                             HCALgranularityPhi = cms.vdouble(0.05, 0.07, 0.10),

                             CHlinear = cms.vdouble(0.00014, 0.00014, 0.0017),
                             CHconstant = cms.vdouble(0.011, 0.017, 0.019),
                             CHMSeta = cms.vdouble(0.0017, 0.0019, 0.0028),
                             CHangularEta = cms.vdouble(0.00030, 0.00035, 0.0042),
                             CHMSphi = cms.vdouble(0.0018, 0.0021, 0.0031),
                             CHangularPhi = cms.vdouble(0.00030, 0.00035, 0.0042))

    return process
