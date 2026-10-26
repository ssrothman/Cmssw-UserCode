from coffea.nanoevents import NanoEventsFactory
import awkward as ak
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import os
import os.path

selected = NanoEventsFactory.from_root('NANO_selected_2.root:Events').events()
dropped = NanoEventsFactory.from_root('NANO_dropped_2.root:Events').events()

events = ak.concatenate([selected, dropped], axis=0)


passGEN = events.NANOTriggerResults.Genselections_path == 1
passRECO = events.NANOTriggerResults.Recoselections_path == 1

# first two (status=1) LHE particles are the Z decay products
LHEparts = events.LHEPart[events.LHEPart.status == 1][:, :2]

LHEp4 = ak.zip({
    'pt': LHEparts.pt,
    'eta': LHEparts.eta,
    'phi': LHEparts.phi,
    'mass': LHEparts.mass,  
}, with_name='PtEtaPhiMLorentzVector')
LHEZ = LHEp4[:,0] + LHEp4[:,1]

passLHE = (np.abs(LHEparts.pdgId[:,0]) == 13) & \
          (np.abs(LHEparts.pdgId[:,1]) == 13) & \
          (ak.max(LHEparts.pt, axis=1) > 26) & \
          (ak.min(LHEparts.pt, axis=1) > 15) & \
          (np.abs(LHEparts.eta[:,0]) < 2.4) & \
          (np.abs(LHEparts.eta[:,1]) < 2.4) & \
          (LHEparts.pdgId[:,0] == -LHEparts.pdgId[:,1]) & \
          (np.abs(LHEZ.mass - 91.1876) < 15)


genparts = events.GenPart[events.GenPart.status == 1]
genparts = genparts[(np.abs(genparts.pdgId) == 13)]
isort = ak.argsort(genparts.pt, ascending=False)
genparts = genparts[isort]
genparts = ak.pad_none(genparts, 2)[:, :2]


recoparts = events.Muon
recomumask = recoparts.pfIsoId >= 2
recomumask = recomumask & (recoparts.mediumId)
recomumask = recomumask & (np.abs(recoparts.dz) < 0.5)
recomumask = recomumask & (np.abs(recoparts.dxy) < 0.2)

recoparts = ak.zip({
    'pt': recoparts.pt,
    'eta': recoparts.eta,
    'phi': recoparts.phi,
    'mass': recoparts.mass,
    'pdgId': recoparts.pdgId
}, with_name='PtEtaPhiMLorentzVector')

recoparts = recoparts[recomumask]
isort = ak.argsort(recoparts.pt, ascending=False)
recoparts = recoparts[isort]
recoparts = ak.pad_none(recoparts, 2)[:, :2]

def compare_event_selection(mask1, mask2, muons1, muons2, name1, name2, basepath):
    os.makedirs(os.path.dirname(basepath), exist_ok=True)
    
    n11 = np.sum(mask1 & mask2)
    n10 = np.sum(mask1 & ~mask2)
    n01 = np.sum(~mask1 & mask2)
    n00 = np.sum(~mask1 & ~mask2)

    confusion_matrix = np.array([[n11, n10], [n01, n00]])

    plt.figure(figsize=(8, 6))

    sns.heatmap(
        confusion_matrix, 
        annot=True, 
        fmt='d', 
        cmap='Blues', 
        xticklabels=['Pass', 'Fail'], 
        yticklabels=['Pass', 'Fail']
    )
    plt.xlabel(f'{name2} Selection')
    plt.ylabel(f'{name1} Selection')

    plt.tight_layout()
    plt.savefig(
        basepath + name1 + '_vs_' + name2 + '_confusion_matrix.png', 
        dpi=300, bbox_inches='tight'
    )

    # diagnose why the selections disagree
    dR_test = muons1[:,0].deltaR(muons2)
    
    argmin_idx = ak.singletons(ak.argmin(dR_test, axis=1))
    argmax_idx = ak.singletons(ak.argmax(dR_test, axis=1))

    matched_first = ak.firsts(muons2[argmin_idx])
    matched_second = ak.firsts(muons2[argmax_idx])

    dR_first = muons1[:,0].deltaR(matched_first)
    dR_second = muons1[:,1].deltaR(matched_second)

    maxDR = dR_first
    maxDR = ak.fill_none(maxDR, -1)

    plt.figure(figsize=(8, 6))
    plt.hist(
        [
            maxDR[mask1 & mask2],
            maxDR[mask1 & ~mask2],
            maxDR[~mask1 & mask2],
            #maxDR[~mask1 & ~mask2]
        ],
        bins = 100,
        histtype = 'step',
        label = [
            'Pass both selections',
            'Pass ' + name1 + ', fail ' + name2,
            'Fail ' + name1 + ', pass ' + name2,
            #'Fail both selections'
        ]
    )
    plt.xlabel('Max $\\Delta R$ between matched muons')
    plt.ylabel('Number of events')
    plt.yscale('log')
    plt.legend()
    plt.tight_layout()
    plt.savefig(
        basepath + name1 + '_vs_' + name2 + '_maxDR_histogram.png', 
        dpi=300, bbox_inches='tight'
    )
 
    dpt1 = (muons1[:,0].pt - matched_first.pt) / matched_first.pt
    dpt2 = (muons1[:,1].pt - matched_second.pt) / matched_second.pt

    dpt1 = ak.fill_none(dpt1, -2)
    dpt2 = ak.fill_none(dpt2, -2)

    plt.figure(figsize=(8, 6))
    plt.hist(
        [
            dpt1[mask1 & mask2],
            dpt1[mask1 & ~mask2],
            dpt1[~mask1 & mask2],
            #dpt1[~mask1 & ~mask2]
        ],
        bins = 100,
        histtype = 'step',
        label = [
            'Pass both selections',
            'Pass ' + name1 + ', fail ' + name2,
            'Fail ' + name1 + ', pass ' + name2,
            #'Fail both selections'
        ]
    )
    plt.xlabel('Relative $\\Delta p_T$ between matched muons')
    plt.ylabel('Number of events')
    plt.yscale('log')
    plt.legend()
    plt.tight_layout()
    plt.savefig(
        basepath + name1 + '_vs_' + name2 + '_dpt_histogram.png', 
        dpi=300, bbox_inches='tight'
    )

    twomuons = ak.sum(~ak.is_none(muons2, axis=1), axis=1) == 2
    leadmupt = (ak.max(muons2.pt, axis=1) > 26) & twomuons
    accmask = twomuons & leadmupt
    submupt = (ak.min(muons2.pt, axis=1) > 15) & accmask
    accmask = accmask & submupt
    leadmueta = (np.abs(muons2.eta[:,0]) < 2.4) & accmask
    accmask = accmask & leadmueta
    submueta = (np.abs(muons2.eta[:,1]) < 2.4) & accmask
    accmask = accmask & submueta
    oppositecharge = (muons2.pdgId[:,0] == -muons2.pdgId[:,1]) & accmask
    accmask = accmask & oppositecharge

    muon2_p4 = ak.zip({
        'pt': muons2.pt,
        'eta': muons2.eta,
        'phi': muons2.phi,
        'mass': muons2.mass,
    }, with_name='PtEtaPhiMLorentzVector')
    Z2 = muon2_p4[:,0] + muon2_p4[:,1]
    Zmass = (np.abs(Z2.mass - 91.1876) < 15) & accmask
    accmask = accmask & Zmass

    xticks = ['total', 'two muons', 'lead $p_T > 26$', 'sublead $p_T > 15$', 'lead $|\\eta| < 2.4$', 'sublead $|\\eta| < 2.4$', 'opposite charge', 'Z mass window']
    x = np.arange(len(xticks))
    y = [
        np.sum(mask1 & ~mask2),
        np.sum(twomuons[mask1 & ~mask2]),
        np.sum(leadmupt[mask1 & ~mask2]),
        np.sum(submupt[mask1 & ~mask2]),
        np.sum(leadmueta[mask1 & ~mask2]),  
        np.sum(submueta[mask1 & ~mask2]),
        np.sum(oppositecharge[mask1 & ~mask2]),
        np.sum(Zmass[mask1 & ~mask2])
    ]

    plt.figure(figsize=(8, 6))
    plt.title(name1 + " but not " + name2)
    bars = plt.bar(x, y)
    plt.bar_label(bars, labels=y, label_type='edge', fontsize=8)
    plt.xticks(x, xticks, rotation=45, ha='right')
    plt.ylabel('Number of events')
    plt.tight_layout()
    plt.savefig(
        basepath + name1 + '_vs_' + name2 + '_acceptance_cuts.png', 
        dpi=300, bbox_inches='tight'
    )

    twomuons = ak.sum(~ak.is_none(muons1, axis=1), axis=1) == 2
    leadmupt = (ak.max(muons1.pt, axis=1) > 26) & twomuons
    accmask = twomuons & leadmupt
    submupt = (ak.min(muons1.pt, axis=1) > 15) & accmask
    accmask = accmask & submupt
    leadmueta = (np.abs(muons1.eta[:,0]) < 2.4) & accmask
    accmask = accmask & leadmueta
    submueta = (np.abs(muons1.eta[:,1]) < 2.4) & accmask
    accmask = accmask & submueta
    oppositecharge = (muons1.pdgId[:,0] == -muons1.pdgId[:,1]) & accmask
    accmask = accmask & oppositecharge

    muon1_p4 = ak.zip({
        'pt': muons1.pt,
        'eta': muons1.eta,
        'phi': muons1.phi,
        'mass': muons1.mass,
    }, with_name='PtEtaPhiMLorentzVector')
    Z1 = muon1_p4[:,0] + muon1_p4[:,1]
    Zmass = (np.abs(Z1.mass - 91.1876) < 15) & accmask
    accmask = accmask & Zmass

    xticks = ['total', 'two muons', 'lead $p_T > 26$', 'sublead $p_T > 15$', 'lead $|\\eta| < 2.4$', 'sublead $|\\eta| < 2.4$', 'opposite charge', 'Z mass window']
    x = np.arange(len(xticks))
    y = [
        np.sum(~mask1 & mask2),
        np.sum(twomuons[~mask1 & mask2]),
        np.sum(leadmupt[~mask1 & mask2]),
        np.sum(submupt[~mask1 & mask2]),
        np.sum(leadmueta[~mask1 & mask2]),  
        np.sum(submueta[~mask1 & mask2]),
        np.sum(oppositecharge[~mask1 & mask2]),
        np.sum(Zmass[~mask1 & mask2])
    ]

    plt.figure(figsize=(8, 6))
    plt.title(name2 + " but not " + name1)
    bars = plt.bar(x, y)
    plt.bar_label(bars, labels=y, label_type='edge', fontsize=8)
    plt.xticks(x, xticks, rotation=45, ha='right')
    plt.ylabel('Number of events')
    plt.tight_layout()
    plt.savefig(
        basepath + name2 + '_vs_' + name1 + '_acceptance_cuts.png', 
        dpi=300, bbox_inches='tight'
    )



compare_event_selection(passLHE, passGEN, LHEparts, genparts, 'LHE', 'GEN', 'plots/')
compare_event_selection(passLHE, passRECO, LHEparts, recoparts, 'LHE', 'RECO', 'plots/')
compare_event_selection(passGEN, passRECO, genparts, recoparts, 'GEN', 'RECO', 'plots/')