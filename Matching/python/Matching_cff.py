import FWCore.ParameterSet.Config as cms
from SRothman.Matching.GenMatchProducer_cfi import *
from SRothman.Matching.GenMatchTableProducer_cfi import *
from SRothman.CustomJets.SimonJetTableProducer_cfi import *

def addGenMatching(process, verbose=0,
                   name='GenMatch',
                   
                   jetMatchingDR = GenMatchProducer.jetMatchingDR,

                   clipval = GenMatchProducer.clipval,

                   spatialLoss = GenMatchProducer.spatialLoss,

                   PUpt0s = GenMatchProducer.PUpt0s,
                   PUexps = GenMatchProducer.PUexps,
                   PUpenalties = GenMatchProducer.PUpenalties,

                   uncertainty = GenMatchProducer.uncertainty,

                   softflavorfilters = GenMatchProducer.softflavorfilters,
                   hardflavorfilters = GenMatchProducer.hardflavorfilters,
                   filterthresholds = GenMatchProducer.filterthresholds,

                   chargefilters = GenMatchProducer.chargefilters,

                   prefitters = GenMatchProducer.prefitters,

                   refiner = GenMatchProducer.refiner,
                   dropGenFilter = GenMatchProducer.dropGenFilter,
                   dropRecoFilter = GenMatchProducer.dropRecoFilter,

                   recoverLostTracks = GenMatchProducer.recoverLostTracks,
                   HADCHrecoverThresholds = GenMatchProducer.HADCHrecoverThresholds,
                   ELErecoverThresholds = GenMatchProducer.ELErecoverThresholds,

                   recoverLostHAD0 = GenMatchProducer.recoverLostHAD0,
                   HAD0recoverThresholds = GenMatchProducer.HAD0recoverThresholds,

                   EMstochastic = GenMatchProducer.EMstochastic,
                   EMconstant = GenMatchProducer.EMconstant,
                   ECALgranularityEta = GenMatchProducer.ECALgranularityEta,
                   ECALgranularityPhi = GenMatchProducer.ECALgranularityPhi,
                   ECALEtaBoundaries = GenMatchProducer.ECALEtaBoundaries,
                   HADstochastic = GenMatchProducer.HADstochastic,
                   HADconstant = GenMatchProducer.HADconstant,
                   HCALgranularityEta = GenMatchProducer.HCALgranularityEta,
                   HCALgranularityPhi = GenMatchProducer.HCALgranularityPhi,
                   HCALEtaBoundaries = GenMatchProducer.HCALEtaBoundaries,
                   CHlinear = GenMatchProducer.CHlinear,
                   CHconstant = GenMatchProducer.CHconstant,
                   CHMSeta = GenMatchProducer.CHMSeta,
                   CHMSphi = GenMatchProducer.CHMSphi,
                   CHangularEta = GenMatchProducer.CHangularEta,
                   CHangularPhi = GenMatchProducer.CHangularPhi,
                   trkEtaBoundaries = GenMatchProducer.trkEtaBoundaries,

                   EM0thresholds = GenMatchProducer.EM0thresholds,
                   HAD0thresholds = GenMatchProducer.HAD0thresholds,
                   HADCHthresholds = GenMatchProducer.HADCHthresholds,
                   ELEthresholds = GenMatchProducer.ELEthresholds,
                   MUthresholds = GenMatchProducer.MUthresholds,

                   EM0dRcuts = GenMatchProducer.EM0dRcuts,
                   HAD0dRcuts = GenMatchProducer.HAD0dRcuts,
                   HADCHdRcuts = GenMatchProducer.HADCHdRcuts,
                   ELEdRcuts = GenMatchProducer.ELEdRcuts,
                   MUdRcuts = GenMatchProducer.MUdRcuts,

                   maxReFit = GenMatchProducer.maxReFit):

    setattr(process, name, 
        GenMatchProducer.clone(
            reco = "SimonJets",
            gen = "GenSimonJets",
            verbose = verbose,

            jetMatchingDR = jetMatchingDR,

            clipval = clipval,

            spatialLoss = spatialLoss,

            PUpt0s = PUpt0s,
            PUexps = PUexps,
            PUpenalties = PUpenalties,

            uncertainty = uncertainty,

            softflavorfilters = softflavorfilters,
            hardflavorfilters = hardflavorfilters,
            filterthresholds = filterthresholds,
            chargefilters = chargefilters,

            prefitters = prefitters,

            refiner = refiner,
            dropGenFilter = dropGenFilter,
            dropRecoFilter = dropRecoFilter,

            recoverLostTracks = recoverLostTracks,
            HADCHrecoverThresholds = HADCHrecoverThresholds,
            ELErecoverThresholds = ELErecoverThresholds,

            recoverLostHAD0 = recoverLostHAD0,
            HAD0recoverThresholds = HAD0recoverThresholds,

            EMstochastic = EMstochastic,
            EMconstant = EMconstant,
            ECALgranularityEta = ECALgranularityEta,
            ECALgranularityPhi = ECALgranularityPhi,
            ECALEtaBoundaries = ECALEtaBoundaries,

            HADstochastic = HADstochastic,
            HADconstant = HADconstant,
            HCALgranularityEta = HCALgranularityEta,
            HCALgranularityPhi = HCALgranularityPhi,
            HCALEtaBoundaries = HCALEtaBoundaries,

            CHlinear = CHlinear,
            CHconstant = CHconstant,
            CHMSeta = CHMSeta,
            CHMSphi = CHMSphi,
            CHangularEta = CHangularEta,
            CHangularPhi = CHangularPhi,
            trkEtaBoundaries = trkEtaBoundaries,

            EM0thresholds = EM0thresholds,
            HAD0thresholds = HAD0thresholds,
            HADCHthresholds = HADCHthresholds,
            ELEthresholds = ELEthresholds,
            MUthresholds = MUthresholds,

            EM0dRcuts = EM0dRcuts,
            HAD0dRcuts = HAD0dRcuts,
            HADCHdRcuts = HADCHdRcuts,
            ELEdRcuts = ELEdRcuts,
            MUdRcuts = MUdRcuts,
        )
    )

    setattr(process, name+"Table", 
        GenMatchTableProducer.clone(
            src = name,
            name = name,
            verbose = verbose
        )
    )

    setattr(process, name+"ParticleTable", 
        SimonJetTableProducer.clone(
            src = 'SimonJets',
            name = name+"Particles",
            verbose = verbose,
            isGen = False,
            addMatch = True,
            matchSrc = name,
            genJets = 'GenSimonJets'
        )
    )

    setattr(process, name+"GenParticleTable",
        SimonJetTableProducer.clone(
            src = 'GenSimonJets',
            name = name+"GenParticles",
            verbose = verbose,
            isGen = True,
            addMatch = True,
            matchSrc = name,
        )
    )

    setattr(process, name+"Task", cms.Task(
        getattr(process, name),
        getattr(process, name+"Table"),
        getattr(process, name+"ParticleTable"),
        getattr(process, name+"GenParticleTable")
    ))
    process.schedule.associate(getattr(process, name+"Task"))

    return process
