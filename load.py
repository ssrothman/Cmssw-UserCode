from coffea.nanoevents import NanoEventsFactory, NanoAODSchema
import awkward as ak
import numpy as np

x = NanoEventsFactory.from_root("NANO_NANO.root", schemaclass=NanoAODSchema).events()

matrix = x.GenMatch.matrix
nrows = ak.flatten(x.GenMatchBK.n_rows)
ncols = ak.flatten(x.GenMatchBK.n_cols)
ntot = nrows * ncols

matrix = ak.unflatten(matrix, ntot, axis=-1)
matrix = ak.unflatten(matrix, np.repeat(ncols, nrows), axis=-1)


wts = x.RecoEECWTS.value
dRs = x.RecoEECDRS.value
nDR = x.RecoEECBK.nDR
nWT = x.RecoEECBK.nWts
nCov = x.RecoEECBK.nCov
nOrd = x.RecoEECBK.nOrders

wts = ak.unflatten(wts, ak.flatten(nWT), axis=-1)
dRs = ak.unflatten(dRs, ak.flatten(nDR), axis=-1)

wtct = np.repeat(ak.flatten(ak.num(dRs, axis=-1)), ak.flatten(nOrd, axis=None))
wts = ak.unflatten(wts, wtct, axis=-1)

cov = x.RecoEECCOV.value
cov = ak.unflatten(cov, ak.flatten(nCov), axis=-1)
covct = np.repeat(ak.flatten(nWT), ak.flatten(nWT))
cov = ak.unflatten(cov, covct, axis=-1)

orders = x.RecoEECWTSBK.order
orders = ak.unflatten(orders, ak.flatten(nOrd), axis=-1)

#for binning cov, the follow tricks will be necessary
covDR, _ = ak.broadcast_arrays(dRs[:,:,None,:], wts)
covDR = ak.flatten(covDR, axis=-1) #can then be broadcasted with cov for binning

#similarly
covOrd = ak.local_index(orders, axis=-1) #for binning, better to have contiguous indices
covOrd, _ = ak.broadcast_arrays(covOrd, wts);
covOrd = ak.flatten(covOrd, axis=-1)
