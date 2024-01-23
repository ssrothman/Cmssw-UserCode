import FWCore.ParameterSet.Config as cms
from SRothman.Matching.GenMatchProducer_cfi import *
from SRothman.Matching.GenMatchTableProducer_cfi import *
from SRothman.CustomJets.SimonJetTableProducer_cfi import *

def addGenMatching(process, verbose=0,
                   name='GenMatch',

                   reco = 'SimonJets',
                   gen = 'GenSimonJets',
                   
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

                   dRfilters = GenMatchProducer.dRfilters,

                   prefitters = GenMatchProducer.prefitters,

                   refiner = GenMatchProducer.refiner,
                   dropGenFilter = GenMatchProducer.dropGenFilter,
                   dropRecoFilter = GenMatchProducer.dropRecoFilter,

                   recoverLostTracks = GenMatchProducer.recoverLostTracks,
                   propagateLostTracks = GenMatchProducer.propagateLostTracks,
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

                   EM0constDR = GenMatchProducer.EM0constDR,
                   EM0floatDR = GenMatchProducer.EM0floatDR,
                   EM0capDR = GenMatchProducer.EM0capDR,

                   HAD0constDR = GenMatchProducer.HAD0constDR,
                   HAD0floatDR = GenMatchProducer.HAD0floatDR,
                   HAD0capDR = GenMatchProducer.HAD0capDR,

                   HADCHconstDR = GenMatchProducer.HADCHconstDR,
                   HADCHfloatDR = GenMatchProducer.HADCHfloatDR,
                   HADCHcapDR = GenMatchProducer.HADCHcapDR,

                   ELEconstDR = GenMatchProducer.ELEconstDR,
                   ELEfloatDR = GenMatchProducer.ELEfloatDR,
                   ELEcapDR = GenMatchProducer.ELEcapDR,

                   MUconstDR = GenMatchProducer.MUconstDR,
                   MUfloatDR = GenMatchProducer.MUfloatDR,
                   MUcapDR = GenMatchProducer.MUcapDR,

                   maxReFit = GenMatchProducer.maxReFit):

    setattr(process, name, 
        GenMatchProducer.clone(
            reco = reco,
            gen = gen,
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

            dRfilters = dRfilters,

            prefitters = prefitters,

            refiner = refiner,
            dropGenFilter = dropGenFilter,
            dropRecoFilter = dropRecoFilter,

            recoverLostTracks = recoverLostTracks,
            HADCHrecoverThresholds = HADCHrecoverThresholds,
            ELErecoverThresholds = ELErecoverThresholds,
            propagateLostTracks = propagateLostTracks,

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

            EM0constDR = EM0constDR,
            EM0floatDR = EM0floatDR,
            EM0capDR = EM0capDR,

            HAD0constDR = HAD0constDR,
            HAD0floatDR = HAD0floatDR,
            HAD0capDR = HAD0capDR,

            HADCHconstDR = HADCHconstDR,
            HADCHfloatDR = HADCHfloatDR,
            HADCHcapDR = HADCHcapDR,

            ELEconstDR = ELEconstDR,
            ELEfloatDR = ELEfloatDR,
            ELEcapDR = ELEcapDR,

            MUconstDR = MUconstDR,
            MUfloatDR = MUfloatDR,
            MUcapDR = MUcapDR,
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
            src = reco,
            name = name+"Particles",
            verbose = verbose,
            isGen = False,
            addMatch = True,
            matchSrc = name,
            genJets = gen
        )
    )

    setattr(process, name+"GenParticleTable",
        SimonJetTableProducer.clone(
            src = gen,
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
