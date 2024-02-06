import FWCore.ParameterSet.Config as cms

orders = []
from SRothman.EECs.EECProducer_cfi import *
for i in range(2, EECProducer.maxOrder.value()+1):
    orders.append(i)
for p1, p2 in zip(EECProducer.p1s, EECProducer.p2s):
    orders.append(p1 + 10*p2)

EECTableProducer = cms.EDProducer("EECTableProducer",
    src = cms.InputTag("EECProducer"),
    name = cms.string("EEC"),
    verbose = cms.int32(1),
    orders = cms.vint32(*orders)
)
