from coffea.nanoevents import NanoEventsFactory
import awkward as ak
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
import os
import os.path
import hist

events = NanoEventsFactory.from_root('NANO_selected_2.root:Events').events()

passGEN = events.NANOTriggerResults.Genselections_path == 1
passRECO = events.NANOTriggerResults.Recoselections_path == 1

passBOTH = passGEN & passRECO

genjets = events.ak8GenJetsNoNu
gensimonjets = events.GenChargedSimonJetsNOMBK
genjetsmatched = gensimonjets.genJetMatched > 0

recojets = events.finalSelectedJetsAK8
recosimonjets = events.ChargedSimonJetsNOMBK
recojetsmatched = recosimonjets.jetMatched > 0

genjets['preselection'] = (genjets.pt > 10.0) & \
                      (np.abs(genjets.eta) < 1.7) & \
                      (genjets.muonEnergy / (genjets.pt * np.cosh(genjets.eta)) < 0.8) & \
                      (genjets.chargedEmEnergy / (genjets.pt * np.cosh(genjets.eta)) < 0.8) & \
                      (genjets.neutralEmEnergy / (genjets.pt * np.cosh(genjets.eta)) < 0.9) & \
                      (genjets.neutralHadronEnergy / (genjets.pt * np.cosh(genjets.eta)) < 0.9) & \
                      (genjets.nConstituents >= 1)

Hmatch_vs_pt = hist.Hist(
    hist.axis.Regular(
        25, 0, 100, 
        name = 'pt', label = 'Jet $p_T$ [GeV]',
        overflow=True, underflow=True
    ),
    hist.axis.Boolean(
        name = 'matched', label = 'matched'
    ),
    storage = hist.storage.Weight()
)

def plot_rate(H, offset = 0, **kwargs):
    Npass = H[{'matched' : True}].values()
    Ntotal = H[{'matched' : slice(None, None, sum)}].values()

    rate = Npass / Ntotal
    centers = H[{'matched' : True}].axes[0].centers
    widths = H[{'matched' : True}].axes[0].widths

    plt.errorbar(
        centers + offset, rate,
        xerr = widths/2, 
        fmt = 'o',
        **kwargs
    )

# matching from 1->2
def evaluate_matching(jets1, jets2, idx12, matched1, matched2, mainmask,
                      name1, name2, fname,
                      ptbin_min, ptbin_max):
    jets1 = jets1[mainmask]
    idx12 = idx12[mainmask]
    matched1 = matched1[mainmask]
    
    # matching rate
    n_matched = ak.sum(matched1)
    total = ak.count(jets1.pt)
    matching_rate = n_matched / total

    # matching rate to jets2 wich pass the preselection
    badmask = idx12  < 0
    idx12 = ak.where(badmask, 0, idx12)
    matched_j2 = jets2[idx12]

    overlap2 = ak.where(badmask, False, matched_j2.overlapVeto)
    presel2 = ak.where(badmask, False, matched_j2.preselection)

    n_matched_presel = ak.sum(matched1 & presel2)
    rate_presel = n_matched_presel / total

    n_matched_overlap = ak.sum(matched1 & overlap2)
    rate_overlap = n_matched_overlap / total

    n_matched_both = ak.sum(matched1 & presel2 & overlap2)
    rate_both = n_matched_both / total

    print("Total matching rate: {:.2f}%".format(matching_rate * 100))
    print("Matching rate with preselection: {:.2f}%".format(rate_presel * 100))
    print("Matching rate with overlap veto: {:.2f}%".format(rate_overlap * 100))
    print("Matching rate with both: {:.2f}%".format(rate_both * 100))


    Hmatch_total = Hmatch_vs_pt.copy().reset()
    Hmatch_total.fill(
        pt = ak.flatten(jets1.pt, axis=None),
        matched = ak.flatten(matched1, axis=None),
    )

    Hmatch_presel = Hmatch_vs_pt.copy().reset()
    Hmatch_presel.fill(
        pt = ak.flatten(jets1.pt, axis=None),
        matched = ak.flatten(matched1 & presel2, axis=None),
    )

    Hmatch_overlap = Hmatch_vs_pt.copy().reset()
    Hmatch_overlap.fill(
        pt = ak.flatten(jets1.pt, axis=None),
        matched = ak.flatten(matched1 & overlap2, axis=None),
    )

    Hmatch_both = Hmatch_vs_pt.copy().reset()
    Hmatch_both.fill(
        pt = ak.flatten(jets1.pt, axis=None),
        matched = ak.flatten(matched1 & presel2 & overlap2, axis=None),
    )

    plt.figure(figsize=(8, 6))

    plt.axvline(10.0, color='purple', linestyle='--', label='Gen threshold', alpha=0.5)
    plt.axvline(20.0, color='magenta', linestyle='--', label='Reco threshold', alpha=0.5)

    plt.fill_betweenx([0, 1], 30, 50, color='gray', alpha=0.2, label='Underflow bin')
    plt.fill_betweenx([0, 1], 50, 90, color='lightblue', alpha=0.2, label='First published bin')

    plt.axhline(1.0, color='black', linestyle='--')
    plt.axhline(0.0, color='black', linestyle='--')

    plot_rate(Hmatch_total, label = 'Total matching rate', color = 'blue', offset=-0.5)
    plot_rate(Hmatch_presel, label = 'Matching rate with preselection', color = 'orange', offset=0.5)
    plot_rate(Hmatch_overlap, label = 'Matching rate with overlap veto', color = 'green', offset=0.0)
    plot_rate(Hmatch_both, label = 'Matching rate with both', color = 'red', offset=1.0)


    plt.xlabel('Jet $p_T$ [GeV]')
    plt.ylabel('Matching rate')
    plt.legend()
    plt.tight_layout()
    plt.savefig(fname, dpi=300, bbox_inches='tight')
    plt.clf()

    if name2 == 'Gen':

        baseline_mask = (jets1.pt > ptbin_min) & (jets1.pt < ptbin_max) & matched1
        ptthresh_mask = ak.where(
            badmask, False, 
            matched_j2.pt > 10.0
        ) & baseline_mask
        muEF_mask = ak.where(
            badmask, False, 
            matched_j2.muonEnergy / (matched_j2.pt * np.cosh(matched_j2.eta)) < 0.8
        ) & ptthresh_mask
        chEmEF_mask = ak.where(
            badmask, False,
            matched_j2.chargedEmEnergy / (matched_j2.pt * np.cosh(matched_j2.eta)) < 0.8
        ) & muEF_mask
        nEmEF_mask = ak.where(
            badmask, False,
            matched_j2.neutralEmEnergy / (matched_j2.pt * np.cosh(matched_j2.eta)) < 0.9
        ) & chEmEF_mask
        nHadEF_mask = ak.where(
            badmask, False,
            matched_j2.neutralHadronEnergy / (matched_j2.pt * np.cosh(matched_j2.eta)) < 0.9
        ) & nEmEF_mask
        nConstit_mask = ak.where(
            badmask, False,
            matched_j2.nConstituents >= 1
        ) & nHadEF_mask
        overlap_mask = ak.where(
            badmask, False,
            matched_j2.overlapVeto
        ) & nConstit_mask
        eta_mask = ak.where(
            badmask, False,
            np.abs(matched_j2.eta) < 1.7
        ) & overlap_mask

        cutflow_masks = [
            baseline_mask,
            ptthresh_mask,
            muEF_mask,
            chEmEF_mask,
            nEmEF_mask,
            nHadEF_mask,
            nConstit_mask,
            overlap_mask,
            eta_mask,
        ]

        cutflow_labels = [ 
            'No cuts at ' + name2 + ' level',
            name2 + ' pt > 10',
            name2 + ' $\\mu$ EF < 0.8',
            name2 + ' $e$ EF < 0.8',
            name2 + ' $\\gamma$ EF < 0.9',
            name2 + ' $h^0$ EF < 0.9',
            name2 + ' $N_{\\text{const}} \\geq 1$',
            name2 + ' $Z\\to \\mu\\mu$ veto',
            name2 + ' $|\\eta|$ < 1.7',
        ]

    elif name2 == 'Reco':
        baseline_mask = (jets1.pt > ptbin_min) & (jets1.pt < ptbin_max) & matched1
        ptthresh_mask = ak.where(
            badmask, False, 
            matched_j2.pt > 20.0
        ) & baseline_mask
        jetID_mask = ak.where(
            badmask, False,
            matched_j2.jetIdLepVeto
        ) & ptthresh_mask
        nConstit_mask = ak.where(
            badmask, False,
            matched_j2.nConstituents >= 2
        ) & jetID_mask
        overlap_mask = ak.where(
            badmask, False,
            matched_j2.overlapVeto
        ) & nConstit_mask
        eta_mask = ak.where(
            badmask, False,
            np.abs(matched_j2.eta) < 1.7
        ) & overlap_mask

        cutflow_masks = [
            baseline_mask,
            ptthresh_mask,
            jetID_mask,
            nConstit_mask,
            overlap_mask,
            eta_mask,
        ]
        cutflow_labels = [ 
            'No cuts at ' + name2 + ' level',
            name2 + ' pt > 20',
            name2 + ' PF jet ID',
            name2 + ' $N_{\\text{const}} \\geq 2$',
            name2 + ' $Z\\to \\mu\\mu$ veto',
            name2 + ' $|\\eta|$ < 1.7',
        ]

    else:
        raise ValueError("name2 must be either 'Gen' or 'Reco'")

    cutflow_total = ak.sum((jets1.pt > ptbin_min) & (jets1.pt < ptbin_max))
    cutflow_rates = [ak.sum(mask)/cutflow_total for mask in cutflow_masks]

    barlabels = ['%0.2f%%' % (rate * 100) for rate in cutflow_rates]

    plt.figure(figsize=(8, 6))

    bars = plt.bar(cutflow_labels, cutflow_rates)
    plt.bar_label(
        bars, labels=barlabels, label_type='edge', fontsize=8
    )
    plt.xticks(rotation=45, ha='right')
    plt.ylabel('Fraction of ' + name1 + ' jets matched to ' + name2 + ' level')
    plt.title(str(ptbin_min) + ' < ' + name1 + ' pt < ' + str(ptbin_max))
    plt.tight_layout()
    plt.savefig(
        'test_cutflow_' + name1 + '_matched_to_' + name2 + 'pt' + str(ptbin_min) + '-' + str(ptbin_max) + '.png', 
        dpi=300, bbox_inches='tight'
    )
    plt.clf()

ptbins = [30.0, 50.0, 90.0, 150.0]

for ipt in range(len(ptbins)-1):
    ptbin_min = ptbins[ipt]
    ptbin_max = ptbins[ipt+1]

    evaluate_matching(
        genjets, recojets, gensimonjets.iReco, genjetsmatched, recojetsmatched, 
        passBOTH & (genjets.overlapVeto) & (genjets.preselection),
        name1 = 'Gen', name2 = 'Reco',
        fname = 'matching_rate_genjets_vs_recojets.png',
        ptbin_min = ptbin_min,
        ptbin_max = ptbin_max
    )
    evaluate_matching(
        recojets, genjets, recosimonjets.iGen, recojetsmatched, genjetsmatched, 
        passBOTH & (recojets.overlapVeto) & (recojets.preselection),
        name1 = 'Reco', name2 = 'Gen',
        fname = 'matching_rate_recojets_vs_genjets.png',
        ptbin_min = ptbin_min,
        ptbin_max = ptbin_max
    )