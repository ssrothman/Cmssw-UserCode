import FWCore.ParameterSet.Config as cms
from  PhysicsTools.NanoAOD.common_cff import *
from SRothman.JetToolbox.jetToolbox_cff import jetToolbox

# ---------------------------------------------------------
# Cusomtization function for custom jets via jet toolbox

def setupCustomizedJetToolbox(process, runOnMC=False, PU="Puppi", cut="", genJetCut=""):
    JECPayload = 'AK4PF'
    if PU=='Puppi' or PU=='CHS':
      JECPayload += PU
    elif PU=="CS":
      JEC+="CHS"
    else:
      print("Unspported PU method")
      return

    btags = [
        'pfDeepCSVJetTags:probb',
        'pfDeepCSVJetTags:probbb',
        'pfDeepCSVJetTags:probc',
        'pfDeepCSVJetTags:probudsg',

        'pfDeepFlavourJetTags:probb',
        'pfDeepFlavourJetTags:probbb',
        'pfDeepFlavourJetTags:problepb',
        'pfDeepFlavourJetTags:probc',
        'pfDeepFlavourJetTags:probuds',
        'pfDeepFlavourJetTags:probg']

    btaginfos = [ 'pfDeepCSVTagInfos', 'pfDeepFlavourTagInfos' ] 

    jetToolbox(process, 'ak4', 'dummyseq', 'noOutput',
               PUMethod = PU, dataTier='nanoAOD',
               runOnMC = runOnMC, 
               JETCorrPayload = JECPayload, JETCorrLevels=['None'], #?
               GetJetMCFlavour=True,
               Cut = cut, genJetCut = genJetCut,
               postFix='',
               bTagDiscriminators = btags, bTagInfos=btaginfos,
               addQGTagger=False, 
               verbosity=4)

    return process

def addPFCands(process, runOnMC=False, jetsName="selectedPatJetsAK4PFPuppi", genJetsName='ak4GenJetsNoNu'): #always save all PFCands, all gen particles, etc
    '''
        Add PFCands and/or jet-PFCand association tables to NanoAOD
        To control which PFCands are saved:
            - If saveAll is True, save all PFcands
            - Otherwise, save the PFCands corresponding to the addAK* options.
        To control which jet-PFCand tables are made, use the addAK* options.
    '''
    process.customizedPFCandsTask = cms.Task()
    process.schedule.associate(process.customizedPFCandsTask)

    candInput = cms.InputTag("puppi")

    # Make constituent table producers
    process.customConstituentsExtTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
                                                        src = candInput,
                                                        cut = cms.string(""), #we should not filter after pruning
                                                        name = cms.string("PFCands"),
                                                        doc = cms.string("interesting particles from various jet collections"),
                                                        singleton = cms.bool(False), # the number of entries is variable
                                                        extension = cms.bool(False), # this is the extension table for the AK8 constituents
                                                        variables = cms.PSet(CandVars,
                                                            puppiWeight = Var("puppiWeight()", float, doc="Puppi weight",precision=10),
                                                            puppiWeightNoLep = Var("puppiWeightNoLep()", float, doc="Puppi weight removing leptons",precision=10),
                                                            vtxChi2 = Var("?hasTrackDetails()?vertexChi2():-1", float, doc="vertex chi2",precision=10),
                                                            trkChi2 = Var("?hasTrackDetails()?pseudoTrack().normalizedChi2():-1", float, doc="normalized trk chi2", precision=10),
                                                            dz = Var("?hasTrackDetails()?dz():-1", float, doc="pf dz", precision=10),
                                                            dzErr = Var("?hasTrackDetails()?dzError():-1", float, doc="pf dz err", precision=10),
                                                            d0 = Var("?hasTrackDetails()?dxy():-1", float, doc="pf d0", precision=10),
                                                            d0Err = Var("?hasTrackDetails()?dxyError():-1", float, doc="pf d0 err", precision=10),
                                                            pvAssocQuality = Var("pvAssociationQuality()", int, doc="primary vertex association quality"),
                                                            lostInnerHits = Var("lostInnerHits()", int, doc="lost inner hits"),
                                                            trkQuality = Var("?hasTrackDetails()?pseudoTrack().qualityMask():0", int, doc="track quality mask"),
                                                         )
                                    )

    process.customAK4ConstituentsTable = cms.EDProducer("PatJetConstituentTableProducer",
                                                        candidates = candInput,
                                                        jets = cms.InputTag(jetsName),
                                                        jet_radius = cms.double(0.4),
                                                        name = cms.string(jetsName+"PFCands"),
                                                        idx_name = cms.string("pFCandsIdx"),
                                                        nameSV = cms.string(jetsName+"SVs"),
                                                        idx_nameSV = cms.string("sVIdx"),
                                                        )

    # Add constituents tables to customizedPFCandsTask
    process.customizedPFCandsTask.add(process.customConstituentsExtTable)
    process.customizedPFCandsTask.add(process.customAK4ConstituentsTable)

    if runOnMC:
        process.genJetsAK4Constituents = cms.EDProducer("GenJetPackedConstituentPtrSelector",
                                                    src = cms.InputTag(genJetsName),
                                                    cut = cms.string("pt > 10")
                                                    )

        genCandInput = cms.InputTag("packedGenParticles")

        process.genJetsParticleTable = cms.EDProducer("SimpleCandidateFlatTableProducer",
                                                         src = genCandInput,
                                                         cut = cms.string(""), #we should not filter after pruning
                                                         name= cms.string("GenCands"),
                                                         doc = cms.string("interesting gen particles from various jet collections"),
                                                         singleton = cms.bool(False), # the number of entries is variable
                                                         extension = cms.bool(False), # this is the main table for the AK8 constituents
                                                         variables = cms.PSet(CandVars)
                                                     )
        process.genAK4ConstituentsTable = cms.EDProducer("GenJetConstituentTableProducer",
                                                            candidates = genCandInput,
                                                            jets = cms.InputTag("genJetsAK4Constituents"), # Note: The name has "Constituents" in it, but these are the jets
                                                            name = cms.string(genJetsName+"Cands"),
                                                            nameSV = cms.string(genJetsName+"JetSVs"),
                                                            idx_name = cms.string("pFCandsIdx"),
                                                            idx_nameSV = cms.string("sVIdx"),
                                                            readBtag = cms.bool(False))

        # Add everything to customizedPFCandsTask
        process.customizedPFCandsTask.add(process.genJetsAK4Constituents) #Note: For gen need to add jets to the process to keep pt cuts.
        process.customizedPFCandsTask.add(process.genJetsParticleTable)
        process.customizedPFCandsTask.add(process.genAK4ConstituentsTable)

    return process
