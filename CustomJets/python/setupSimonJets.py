import FWCore.ParameterSet.Config as cms

from SRothman.CustomJets.SimonJetProducer_cfi import GenSimonJetProducer, PatSimonJetProducer
from SRothman.CustomJets.SimonJetTableProducer_cfi import SimonJetTableProducer
from SRothman.Analysis.util import pyval_to_cmsval

def syst_params_from_config(config):
    result = cms.PSet()
    for key, value in config['parameters'].items():
        result.__setattr__(key, pyval_to_cmsval(value))
    return result

def syst_settings_from_config(config, syst):
    result = cms.PSet()
    for key, value in config['nominal'].items():
        result.__setattr__(key, pyval_to_cmsval(value))

    # Apply systematic variations
    if syst in config['variations']:
        for key, value in config['variations'][syst].items():
            result.__setattr__(key, pyval_to_cmsval(value))
    
        print("Settings for systematic variation %s:" % syst)
        for key in result.parameterNames_():
            print("  %s: %s" % (key, getattr(result, key)))

    elif syst != 'NOM':
        raise ValueError("Systematic variation %s not found in config." % syst)
    
    return result

def selector_from_config(config, syst):
    parameters = syst_params_from_config(config)
    settings = syst_settings_from_config(config, syst)
    return cms.PSet(
        parameters = parameters,
        settings = settings,
    )

def setupGenSimonJets(process,
                      genjets,
                      name,
                      config):

    setattr(process, 'Gen'+name, GenSimonJetProducer.clone(
        jetSrc = genjets,
        addCHSindex = False,
        verbose = False,
        selector = selector_from_config(config['Systematics'], 'NOM')
    ))

    setattr(process, 'Gen'+name+'Table', SimonJetTableProducer.clone(
        src = 'Gen'+name,
        name = 'Gen'+name,
        verbose=False,
    ))

    setattr(process, name+"MCTask", cms.Task(
        getattr(process, 'Gen'+name),
        getattr(process, 'Gen'+name+'Table')
    ))
    process.schedule.associate(getattr(process, name+'MCTask'))

    return process

def setupRecoSimonJets(process,
                       jets,
                       CHSjets,
                       name,
                       config,
                       syst):

    doCHS = len(CHSjets) > 0

    setattr(process, name, PatSimonJetProducer.clone(
        jetSrc = jets,
        CHSsrc = CHSjets,
        addCHSindex = doCHS,
        CHSmatchDR = config['Jets']['CHSmatchDR'],
        verbose = False,
        selector = selector_from_config(config['Systematics'], syst)
    ))

    setattr(process, name+"Preselection", cms.EDProducer("JetSelectionFlagTranslator",
        src = cms.InputTag(jets),
        target = cms.InputTag(name),
        map = cms.InputTag("preselectJetsAK8"),
        verbose = cms.int32(0)
    ))

    setattr(process, name+"OverlapVeto", cms.EDProducer("JetSelectionFlagTranslator",
        src = cms.InputTag(jets),
        target = cms.InputTag(name),
        map = cms.InputTag("overlapVetoJetsAK8"),
        verbose = cms.int32(0)
    ))

    setattr(process, name+'Table', SimonJetTableProducer.clone(
        src = name,
        name = name,
        verbose=False,
    ))

    setattr(process, name+"CHSTable", cms.EDProducer("CHSSumTableProducer",
        src = cms.InputTag(name),
        name = cms.string(name+"BK"),
        CHSsrc = cms.InputTag(CHSjets),
        verbose = cms.int32(0)
    ))

    setattr(process, name+'Task', cms.Task(
        getattr(process, name),
        getattr(process, name+'Preselection'),
        getattr(process, name+'OverlapVeto'),
        getattr(process, name+'Table'),
        getattr(process, name+'CHSTable')
    ))
    process.schedule.associate(getattr(process, name+'Task'))

    return process

def setupSimonJets(process, 
                   jets, 
                   genjets,
                   CHSjets,
                   name,
                   config,
                   syst,
                   isMC,
                   genOnly):
    if isMC:
        process = setupGenSimonJets(process,
                                    genjets,
                                    name,
                                    config)
    if not genOnly:
        process = setupRecoSimonJets(process,
                                     jets,
                                     CHSjets,
                                     name,
                                     config,
                                     syst)
    return process



