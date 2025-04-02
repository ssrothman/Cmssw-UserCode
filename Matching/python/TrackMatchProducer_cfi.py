import FWCore.ParameterSet.Config as cms

TrackMatchProducer = cms.EDProducer('TrackMatchProducer',
    recoJets = cms.InputTag(''),
    genJets = cms.InputTag(''),
    matcher = cms.PSet(
        jet_dR_threshold = cms.double(0.4),
        max_chisq = cms.double(999999.0),
        Electrons = cms.PSet(
            dr_mode = cms.string("Const"),
            dr_param1 = cms.double(0.08),
            dr_param2 = cms.double(0.0),
            dr_param3 = cms.double(0.0),
            ptres_mode = cms.string("ConstFrac"),
            ptres_param1 = cms.double(0.02),
            ptres_param2 = cms.double(0.0),
            angres_mode = cms.string("TrackAng"),
            angres_param1 = cms.double(0.0003),
            angres_param2 = cms.double(0.0015),
            opp_charge_penalty = cms.double(3.0),
            no_charge_penalty = cms.double(0.0),
            charge_filter_mode = cms.string("Any"),
            flavor_filter_mode = cms.string("Any")
        ),
        Muons = cms.PSet(
            dr_mode = cms.string("Const"),
            dr_param1 = cms.double(0.08),
            dr_param2 = cms.double(0.0),
            dr_param3 = cms.double(0.0),
            ptres_mode = cms.string("ConstFrac"),
            ptres_param1 = cms.double(0.02),
            ptres_param2 = cms.double(0.0),
            angres_mode = cms.string("TrackAng"),
            angres_param1 = cms.double(0.0003),
            angres_param2 = cms.double(0.0015),
            opp_charge_penalty = cms.double(3.0),
            no_charge_penalty = cms.double(0.0),
            charge_filter_mode = cms.string("Any"),
            flavor_filter_mode = cms.string("Any")
        ),
        ChargedHadrons = cms.PSet(
            dr_mode = cms.string("Const"),
            dr_param1 = cms.double(0.08),
            dr_param2 = cms.double(0.0),
            dr_param3 = cms.double(0.0),
            ptres_mode = cms.string("ConstFrac"),
            ptres_param1 = cms.double(0.02),
            ptres_param2 = cms.double(0.0),
            angres_mode = cms.string("TrackAng"),
            angres_param1 = cms.double(0.0003),
            angres_param2 = cms.double(0.0015),
            opp_charge_penalty = cms.double(3.0),
            no_charge_penalty = cms.double(0.0),
            charge_filter_mode = cms.string("Any"),
            flavor_filter_mode = cms.string("Any")
        )
    )
)
