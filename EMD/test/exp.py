import awkward as ak
import numpy as np
from coffea.nanoevents.methods import vector
import uproot
from scipy.special import comb
import coffea
from coffea.nanoevents import NanoEventsFactory, NanoAODSchema, BaseSchema
import matplotlib.pyplot as plt
import hist

import wasserstein

def printmatrix(mat):
  string = ""
  for i in range(mat.shape[1]):
    rowsum=0
    for j in range(mat.shape[0]):
      string += "%0.3f\t"%flows[j,i]
      rowsum += flows[j,i]
    string+='\t%0.3f\n'%rowsum
  string+='\n'
  for i in range(mat.shape[0]):
    colsum = np.sum(mat[:,i])
    string+='%0.3f\t'%colsum
  string+='\n'
  print(string)


t = NanoEventsFactory.from_root("nano_mc2017.root", schemaclass=NanoAODSchema).events()

jet0 = t.selectedPatJetsAK4PFPuppi[0,0]
genJet0 = t.ak4GenJetsNoNu[0,0]

cands0 = t.selectedPatJetsAK4PFPuppiPFCands[0]
cands0 = cands0[cands0.jetIdx==0]
cands0 = t.PFCands[0, cands0.pFCandsIdx]

gencands0 = t.ak4GenJetsNoNuCands[0]
gencands0 = gencands0[gencands0.jetIdx==0]
gencands0 = t.GenCands[0, gencands0.pFCandsIdx]

E0 = ak.to_numpy(cands0.pt/jet0.pt)
eta0 = ak.to_numpy(cands0.eta)
phi0 = ak.to_numpy(cands0.phi)
coords0 = np.stack((eta0, phi0)).transpose()

genE0 = ak.to_numpy(gencands0.pt/genJet0.pt)
geneta0 = ak.to_numpy(gencands0.eta)
genphi0 = ak.to_numpy(gencands0.phi)
gencoords0 = np.stack((geneta0, genphi0)).transpose()

EMD = wasserstein.EMDYPhi(R=0.4)
EMD.preprocess_CenterWeightedCentroid()

EMD(E0, coords0, genE0, gencoords0)

print("EMD =", EMD.emd())
flows = EMD.flows()
print()
print("flow shape:", flows.shape)
printmatrix(flows)


print("RECO:")
printmatrix(E0[None,:])
print()

print("GEN:")
printmatrix(genE0[None,:])
print()

print("FLOW^T x GEN")
printmatrix(np.matmul(flows, genE0[None,:]))
