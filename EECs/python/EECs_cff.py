import FWCore.ParameterSet.Config as cms

def addEECs(process, runOnMC=False, jetName="ak4PFJetsPuppi", genJetName="ak4GenJetsNoNu", maxOrder=2, doFull3Pt=True, doFull4Pt=True, minJetPt=30):
    process.EECTask = cms.Task()
    process.schedule.associate(process.EECTask)

    for order in range(2, maxOrder+1):
        setattr(process, "EEC%dTable"%order, 
            cms.EDProducer("ProjectedEECTableProducer",
                jets = cms.InputTag(jetName),
                order=cms.uint32(order),
                name=cms.string("EEC%d"%order),
                minJetPt=cms.double(minJetPt),
                muons = cms.InputTag('finalMuons')
            )
        )
        process.EECTask.add(getattr(process, "EEC%dTable"%order))
    
    if doFull3Pt:
        process.Full3PtEECTable = cms.EDProducer("Full3PtEECTableProducer",
            jets = cms.InputTag(jetName),
            order = cms.uint32(3),
            name = cms.string("Full3PtEEC"),
            minJetPt=cms.double(minJetPt),
            muons = cms.InputTag('finalMuons')
        )
        process.EECTask.add(process.Full3PtEECTable)
    
    if doFull4Pt:
        process.Full4PtEECTable = cms.EDProducer("Full4PtEECTableProducer",
            jets = cms.InputTag(jetName),
            order = cms.uint32(4),
            name = cms.string("Full4PtEEC"),
            minJetPt=cms.double(minJetPt),
            muons = cms.InputTag('finalMuons')
        )
        process.EECTask.add(process.Full4PtEECTable)

    if runOnMC:
        process.genEECTask = cms.Task()
        process.schedule.associate(process.genEECTask)

        for order in range(2, maxOrder+1):
            setattr(process, "GenEEC%dTable"%order, 
                cms.EDProducer("GenProjectedEECTableProducer",
                    jets = cms.InputTag(genJetName),
                    order=cms.uint32(order),
                    name=cms.string("genEEC%d"%order),
                    minJetPt=cms.double(minJetPt),
                    muons=cms.InputTag('finalMuons')
                )
            )
            process.EECTask.add(getattr(process, "GenEEC%dTable"%order))

        if doFull3Pt:
            process.GenFull3PtEECTable = cms.EDProducer("GenFull3PtEECTableProducer",
                jets = cms.InputTag(genJetName),
                order = cms.uint32(3),
                name = cms.string("genFull3PtEEC"),
                minJetPt=cms.double(minJetPt),
                muons = cms.InputTag('finalMuons')
            )
            process.EECTask.add(process.GenFull3PtEECTable)
        
        if doFull4Pt:
            process.GenFull4PtEECTable = cms.EDProducer("GenFull4PtEECTableProducer",
                jets = cms.InputTag(genJetName),
                order = cms.uint32(4),
                name = cms.string("genFull4PtEEC"),
                minJetPt=cms.double(minJetPt),
                muons = cms.InputTag('finalMuons')
            )
            process.EECTask.add(process.GenFull4PtEECTable)



    return process
