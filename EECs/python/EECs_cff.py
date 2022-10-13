import FWCore.ParameterSet.Config as cms

def addEECs(process, runOnMC=False, jetName="ak4PFJetsPuppi", genJetName="ak4GenJetsNoNu", maxOrder=2, doFull3Pt=True, doFull4Pt=True, minJetPt=10, doNonIRC=True, prefix='Puppi'):
    process.EECTask = cms.Task()
    process.schedule.associate(process.EECTask)

    for order in range(2, maxOrder+1):
        setattr(process, "%sEEC%dTable"%(prefix,order), 
            cms.EDProducer("PatProjectedEECTableProducer",
                jets = cms.InputTag(jetName),
                order=cms.uint32(order),
                name=cms.string("%sEEC%d"%(prefix,order)),
                minJetPt=cms.double(minJetPt),
                muons = cms.InputTag('finalMuons'),
                p1 = cms.uint32(1),
                p2 = cms.uint32(1),
                verbose = cms.uint32(0),
            )
        )
        process.EECTask.add(getattr(process, "%sEEC%dTable"%(prefix,order)))
    
    setattr(getattr(process, '%sEEC2Table'%prefix), 'verbose', cms.uint32(1))

    if doFull3Pt:
        setattr(process,"%sFull3PtEECTable"%prefix,
            cms.EDProducer("PatFull3PtEECTableProducer",
              verbose = cms.uint32(0),
              p1 = cms.uint32(1),
              p2 = cms.uint32(1),
              jets = cms.InputTag(jetName),
              order = cms.uint32(3),
              name = cms.string("%sFull3PtEEC"%prefix),
              minJetPt=cms.double(minJetPt),
              muons = cms.InputTag('finalMuons')
          )
        )
        process.EECTask.add(getattr(process, "%sFull3PtEECTable"%prefix))
    
    if doFull4Pt:
        setattr(process, "%sFull4PtEECTable"%prefix,
            cms.EDProducer("PatFull4PtEECTableProducer",
              verbose = cms.uint32(0),
              p1 = cms.uint32(1),
              p2 = cms.uint32(1),
              jets = cms.InputTag(jetName),
              order = cms.uint32(4),
              name = cms.string("%sFull4PtEEC"%prefix),
              minJetPt=cms.double(minJetPt),
              muons = cms.InputTag('finalMuons')
          )
        )
        process.EECTask.add(getattr(process, "%sFull4PtEECTable"%prefix))

    if doNonIRC:
        setattr(process, "%sEECnonIRC12Table"%prefix,
            cms.EDProducer("PatProjectedEECTableProducer",
              verbose = cms.uint32(0),
              p1 = cms.uint32(1),
              p2 = cms.uint32(2),
              jets = cms.InputTag(jetName),
              order = cms.uint32(2),
              name = cms.string("%sEECnonIRC12"%prefix),
              minJetPt=cms.double(minJetPt),
              muons = cms.InputTag('finalMuons')
          )
        )
        process.EECTask.add(getattr(process, "%sEECnonIRC12Table"%prefix))

        setattr(process, "%sEECnonIRC13Table"%prefix,
            cms.EDProducer("PatProjectedEECTableProducer",
              p1 = cms.uint32(1),
              verbose = cms.uint32(0),
              p2 = cms.uint32(3),
              jets = cms.InputTag(jetName),
              order = cms.uint32(2),
              name = cms.string("%sEECnonIRC13"%prefix),
              minJetPt=cms.double(minJetPt),
              muons = cms.InputTag('finalMuons')
          )
        )
        process.EECTask.add(getattr(process, "%sEECnonIRC13Table"%prefix))

        setattr(process, "%sEECnonIRC22Table"%prefix,
            cms.EDProducer("PatProjectedEECTableProducer",
              verbose = cms.uint32(0),
              p1 = cms.uint32(2),
              p2 = cms.uint32(2),
              jets = cms.InputTag(jetName),
              order = cms.uint32(2),
              name = cms.string("%sEECnonIRC22"%prefix),
              minJetPt=cms.double(minJetPt),
              muons = cms.InputTag('finalMuons')
          )
        )
        process.EECTask.add(getattr(process, "%sEECnonIRC22Table"%prefix))

    if runOnMC:
        process.genEECTask = cms.Task()
        process.schedule.associate(process.genEECTask)

        for order in range(2, maxOrder+1):
            setattr(process, "GenEEC%dTable"%order, 
                cms.EDProducer("GenProjectedEECTableProducer",
                    p1 = cms.uint32(1),
                    verbose = cms.uint32(0),
                    p2 = cms.uint32(1),
                    jets = cms.InputTag(genJetName),
                    order=cms.uint32(order),
                    name=cms.string("genEEC%d"%order),
                    minJetPt=cms.double(minJetPt),
                    muons=cms.InputTag('finalMuons')
                )
            )
            process.genEECTask.add(getattr(process, "GenEEC%dTable"%order))

        setattr(getattr(process, 'GenEEC2Table'), 'verbose', cms.uint32(1))

        if doFull3Pt:
            process.GenFull3PtEECTable = cms.EDProducer("GenFull3PtEECTableProducer",
                verbose = cms.uint32(0),
                p1 = cms.uint32(1),
                p2 = cms.uint32(1),
                jets = cms.InputTag(genJetName),
                order = cms.uint32(3),
                name = cms.string("genFull3PtEEC"),
                minJetPt=cms.double(minJetPt),
                muons = cms.InputTag('finalMuons')
            )
            process.genEECTask.add(process.GenFull3PtEECTable)
        
        if doFull4Pt:
            process.GenFull4PtEECTable = cms.EDProducer("GenFull4PtEECTableProducer",
                p1 = cms.uint32(1),
                verbose = cms.uint32(0),
                p2 = cms.uint32(1),
                jets = cms.InputTag(genJetName),
                order = cms.uint32(4),
                name = cms.string("genFull4PtEEC"),
                minJetPt=cms.double(minJetPt),
                muons = cms.InputTag('finalMuons')
            )
            process.genEECTask.add(process.GenFull4PtEECTable)

        if doNonIRC:
          process.genEECnonIRC12Table = cms.EDProducer("GenProjectedEECTableProducer",
              p1 = cms.uint32(1),
              p2 = cms.uint32(2),
              verbose = cms.uint32(0),
              jets = cms.InputTag(genJetName),
              order = cms.uint32(2),
              name = cms.string("genEECnonIRC12"),
              minJetPt=cms.double(minJetPt),
              muons = cms.InputTag('finalMuons')
          )
          process.genEECTask.add(process.genEECnonIRC12Table)

          process.genEECnonIRC13Table = cms.EDProducer("GenProjectedEECTableProducer",
              p1 = cms.uint32(1),
              verbose = cms.uint32(0),
              p2 = cms.uint32(3),
              jets = cms.InputTag(genJetName),
              order = cms.uint32(2),
              name = cms.string("genEECnonIRC13"),
              minJetPt=cms.double(minJetPt),
              muons = cms.InputTag('finalMuons')
          )
          process.genEECTask.add(process.genEECnonIRC13Table)

          process.genEECnonIRC22Table = cms.EDProducer("GenProjectedEECTableProducer",
              p1 = cms.uint32(2),
              verbose = cms.uint32(0),
              p2 = cms.uint32(2),
              jets = cms.InputTag(genJetName),
              order = cms.uint32(2),
              name = cms.string("genEECnonIRC22"),
              minJetPt=cms.double(minJetPt),
              muons = cms.InputTag('finalMuons')
          )
          process.genEECTask.add(process.genEECnonIRC22Table)

    return process
