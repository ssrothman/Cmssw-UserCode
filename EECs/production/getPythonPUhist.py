import uproot

def getPythonPUhist(filename, histname):
    f = uproot.open(filename)
    h = f[histname]
    return h.to_hist()

HA = getPythonPUhist("PUhist2018A.root", "pileup")
HB = getPythonPUhist("PUhist2018B.root", "pileup")
HC = getPythonPUhist("PUhist2018C.root", "pileup")
HD = getPythonPUhist("PUhist2018D.root", "pileup")

Htotal = HA + HB + HC + HD

import matplotlib.pyplot as plt
HA.plot(density=True, label='2018A')
HB.plot(density=True, label='2018B')
HC.plot(density=True, label='2018C')
HD.plot(density=True, label='2018D')
Htotal.plot(density=True, label='2018 total')
plt.legend()
plt.show()

for fname, H in zip(['PUhist2018A.pkl', 'PUhist2018B.pkl', 'PUhist2018C.pkl', 'PUhist2018D.pkl', 'PUhist2018UL.pkl'], [HA, HB, HC, HD, Htotal]):
    with open(fname, "wb") as f:
        import pickle
        pickle.dump(H, f)
