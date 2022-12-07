import FWCore.ParameterSet.Config as cms

def addEECs(process, name, order, isMC,
            jets, EECParts, genJets=None, genEECParts=None, muons=None, requireZ=True, 
            p1=1, p2=1, 
            verbose=0, 
            minPartPt=0.0,
            flow="EMDFlow"):

    producers = []
    setattr(process, name, cms.EDProducer("ProjectedEECProducer",
        EECParts = cms.InputTag(EECParts),
        order = cms.uint32(order),
        p1 = cms.uint32(p1),
        p2 = cms.uint32(p2),
        minPartPt = cms.double(minPartPt),
    ))
    producers.append(getattr(process, name))

    setattr(process, name+"Table", cms.EDProducer("ProjectedEECTableProducer",
        name = cms.string(name),
        jets = cms.InputTag(EECParts),
        EECs = cms.InputTag(name),
        nDR = cms.uint32(1),
        flow = cms.InputTag(flow),
        isGen = cms.bool(False),
        doParts = cms.bool(False)
    ))
    producers.append(getattr(process, name+"Table"))

    if isMC:
        setattr(process, "gen"+name, cms.EDProducer("ProjectedEECProducer",
            EECParts = cms.InputTag(genEECParts),
            order = cms.uint32(order),
            p1 = cms.uint32(p1),
            p2 = cms.uint32(p2),
            minPartPt = cms.double(minPartPt),
        ))
        producers.append(getattr(process, "gen"+name))

        setattr(process, "gen"+name+"Table", cms.EDProducer("ProjectedEECTableProducer",
            name = cms.string("gen"+name),
            jets = cms.InputTag(genEECParts),
            EECs = cms.InputTag("gen"+name),
            nDR = cms.uint32(1),
            doParts = cms.bool(False),
            flow = cms.InputTag(flow),
            isGen = cms.bool(True)
        ))
        producers.append(getattr(process, "gen"+name+"Table"))

        setattr(process, name+"Transfer", cms.EDProducer("PatProjectedEECTransferProducer",
            jets = cms.InputTag(jets),
            genJets = cms.InputTag(genJets),
            nDR = cms.uint32(1),
            EECs = cms.InputTag(name),
            genEECs = cms.InputTag("gen"+name),
            flows = cms.InputTag(flow),
            mode = cms.string("tuples")
        ))
        producers.append(getattr(process, name+"Transfer"))

        setattr(process, name+"TransferTable", cms.EDProducer("EECTransferTableProducer",
            name = cms.string(name+"Transfer"),
            transfer = cms.InputTag(name+"Transfer")
        ))
        producers.append(getattr(process, name+"TransferTable"))

    setattr(process, name+"Task", cms.Task(*producers))
    process.schedule.associate(getattr(process, name+"Task"))

    return process
