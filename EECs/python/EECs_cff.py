import FWCore.ParameterSet.Config as cms

def addEECs(process, runOnMC=False, jetName="ak4PFJetsPuppi", genJetName="ak4GenJetsNoNu", maxOrder=2):
    process.EECTask = cms.Task()
    process.schedule.associate(process.EECTask)

    for order in range(2, maxOrder+1):
        setattr(process, "EEC%dTable"%order, 
            cms.EDProducer("EECTableProducer",
                jets = cms.InputTag(jetName),
                order=cms.uint32(order),
                name=cms.string("EEC%d"%order),
                minJetPt=cms.double(30),
                muons = cms.InputTag('finalMuons')
            )
        )
        process.EECTask.add(getattr(process, "EEC%dTable"%order))

    if runOnMC:
        process.genEECTask = cms.Task()
        process.schedule.associate(process.genEECTask)

        for order in range(2, maxOrder+1):
            setattr(process, "GenEEC%dTable"%order, 
                cms.EDProducer("GenEECTableProducer",
                    jets = cms.InputTag(genJetName),
                    order=cms.uint32(order),
                    name=cms.string("genEEC%d"%order),
                    minJetPt=cms.double(30),
                    muons=cms.InputTag('finalMuons')
                )
            )
            process.EECTask.add(getattr(process, "GenEEC%dTable"%order))

    return process
