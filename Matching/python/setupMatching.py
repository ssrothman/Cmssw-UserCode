import FWCore.ParameterSet.Config as cms

def setupMatching(process, verbose=0,
                  ak8=True,
                  name='GenMatch',
                  reco = 'SimonJets',
                  gen = 'GenSimonJets',
                  naive=False):
                   
    setattr(process, name, 
        cms.EDProducer("TrackMatchProducer",
            recoJets = cms.InputTag(reco),
            genJets = cms.InputTag(gen),
            matcher = cms.PSet(
                jet_dr_mode = cms.string("Const"),
                jet_dr_param1 = cms.double(0.4),
                jet_dr_param2 = cms.double(0.0),
                jet_dr_param3 = cms.double(0.0),
                jet_ptres_mode = cms.string("Const"),
                jet_ptres_param1 = cms.double(10.0),
                jet_ptres_param2 = cms.double(0.0),
                jet_angres_mode = cms.string("Const"),
                jet_angres_param1 = cms.double(0.4),
                jet_angres_param2 = cms.double(0.0),
                particle_dr_mode = cms.string("Const"),
                particle_dr_param1 = cms.double(0.4),
                particle_dr_param2 = cms.double(0.0),
                particle_dr_param3 = cms.double(0.0),
                particle_ptres_mode = cms.string("Const"),
                particle_ptres_param1 = cms.double(10.0),
                particle_ptres_param2 = cms.double(0.0),
                particle_angres_mode = cms.string("Const"),
                particle_angres_param1 = cms.double(0.4),
                particle_angres_param2 = cms.double(0.0),
                opp_charge_penalty = cms.double(0.0),
                no_charge_penalty = cms.double(0.0),
            )
        )
    )

    setattr(process, name+"Table", 
        cms.EDProducer("TrackMatchTableProducer",
            genname = cms.string(gen),
            reconame = cms.string(reco),
            recoJets = cms.InputTag(reco),
            genJets = cms.InputTag(gen),
            matches = cms.InputTag(name),
        )
    )

    setattr(process, name+"Task", cms.Task(
        getattr(process, name),
        getattr(process, name+"Table"),
    ))
    process.schedule.associate(getattr(process, name+"Task"))

    return process
