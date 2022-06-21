import FWCore.ParameterSet.Config as cms

def addEECs(process, runOnMC=False, order=2, jetName="ak4PFJetsPuppi"):
    process.EECTable2 = cms.EDProducer("EECTableProducer",
        jets = cms.InputTag("ak4PFJetsPuppi"),
        order=cms.uint32(2),
        name=cms.string("EEC2")
    )

    process.EECTable3 = cms.EDProducer("EECTableProducer",
        jets = cms.InputTag("ak4PFJetsPuppi"),
        order=cms.uint32(3),
        name=cms.string("EEC3")
    )

    process.EECTable4 = cms.EDProducer("EECTableProducer",
        jets = cms.InputTag("ak4PFJetsPuppi"),
        order=cms.uint32(4),
        name=cms.string("EEC4")
    )

    process.EECTask = cms.Task(process.EECTable2, process.EECTable3, process.EECTable4)

    process.schedule.associate(process.EECTask)

    return process
