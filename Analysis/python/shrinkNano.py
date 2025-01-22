import FWCore.ParameterSet.Config as cms

def shrink_nanoAOD_MC(process):
    process.nanoSequenceMC.remove(process.ttbarCategoryTable)
    process.nanoSequenceMC.remove(process.boostedTauTables)
    process.nanoSequenceMC.remove(process.boostedTauMC)
    process.nanoSequenceMC.remove(process.lowPtElectronTables)
    process.nanoSequenceMC.remove(process.lowPtElectronMC)
    process.nanoSequenceMC.remove(process.isoTrackTables)
    process.nanoSequenceMC.remove(process.isoTrackSequence)
    #process.nanoSequenceMC.remove(process.tauTables)

    process.particleLevelTables.remove(process.HTXSCategoryTable)
    process.particleLevelTables.remove(process.rivetPhotonTable)
    process.particleLevelTables.remove(process.rivetLeptonTable)

    process.jetTables.remove(process.saJetTable)
    process.jetTables.remove(process.saTable)
    process.jetTables.remove(process.subJetTable)

    process.jetMC.remove(process.subjetMCTable)
    process.jetMC.remove(process.genSubJetAK8Table)

    process.tauMC.remove(process.tauMCTable)
    process.tauMC.remove(process.tausMCMatchHadTauForTable)
    process.tauMC.remove(process.tausMCMatchLepTauForTable)

    return process

def shrink_nanoAOD_data(process):
    process.nanoSequenceCommon.remove(process.boostedTauTables)
    process.nanoSequenceCommon.remove(process.boostedTauSequence)
    process.nanoSequenceCommon.remove(process.isoTrackSequence)
    process.nanoSequenceCommon.remove(process.isoTrackTables)

    process.nanoSequenceOnlyData.remove(process.protonTables)

    process.jetTables.remove(process.saJetTable)
    process.jetTables.remove(process.saTable)
    process.jetTables.remove(process.subJetTable)

    return process
