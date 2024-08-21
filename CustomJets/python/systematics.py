import FWCore.ParameterSet.Config as cms

systematics = cms.PSet(
    EM0scale = cms.double(0.03),
    HAD0scale = cms.double(0.05),
    CHscale = cms.double(0.01),

    trkDropProb = cms.double(0.03),
    trkDropSmear = cms.double(0.10),

    DRtrkDropProbs = cms.vdouble(0.0),
    DRtrkDropEdges = cms.vdouble(0.0, 999999999999999999),

    pDropEM0 = cms.vdouble(0.0),
    pDropHAD0 = cms.vdouble(0.0),
    pDropHADCH = cms.vdouble(0.0),
    pDropELE = cms.vdouble(0.0),
    pDropMU = cms.vdouble(0.0),
    pDropEdges = cms.vdouble(0.0, 999999999999999999),
)

syst = cms.string('NOM')

'''
Available systematics:
 - NOM: nominal
 - EM0_UP: EM0scale + 1 sigma
 - EM0_DOWN: EM0scale - 1 sigma
 - HAD0_UP: HAD0scale + 1 sigma
 - HAD0_DOWN: HAD0scale - 1 sigma
 - CH_UP: CHscale + 1 sigma
 - CH_DOWN: CHscale - 1 sigma
 - TRK_EFF: drop an extra 3% of tracks
 - TRK_EFF_DR: drop extra tracks in a dR-dependent way
 - RECO_EFF: monkey with reconstruction efficiencies

'''
