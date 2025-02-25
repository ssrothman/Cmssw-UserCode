import FWCore.ParameterSet.Config as cms

from res4calculator_cfi import res4transfercalculator

EECRes4TransferProducer = cms.EDProducer("EECRes4TransferProducer",
    genJets = cms.InputTag(""),
    recoJets = cms.InputTag(""),
    matches = cms.InputTag(""),
    calculator = res4transfercalculator
)
