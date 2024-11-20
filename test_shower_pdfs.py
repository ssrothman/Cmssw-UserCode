import argparse

parser=argparse.ArgumentParser()
parser.add_argument('input', help='input file')
args=parser.parse_args()

import matplotlib.pyplot as plt

with open(args.input, 'r') as f:
    lines = f.readlines()

#the first 8 lines are configuration information

zs = []
thetas = []
phis = []
alphas = []
a12s = []
am1s = []
am2s = []
ad1s = []
ad2s = []
adms = []
addms = []
for line in lines[8:]:
    splitted = line.split(',\t')
    zs.append(float(splitted[0]))
    thetas.append(float(splitted[1]))
    phis.append(float(splitted[2]))
    alphas.append(float(splitted[3]))
    a12s.append(float(splitted[4]))
    am1s.append(float(splitted[5]))
    am2s.append(float(splitted[6]))
    ad1s.append(float(splitted[7]))
    ad2s.append(float(splitted[8]))
    adms.append(float(splitted[9]))
    addms.append(float(splitted[10]))

import numpy as np
zs = np.array(zs)
thetas = np.array(thetas)
phis = np.array(phis)
alphas = np.array(alphas)
a12s = np.array(a12s)
am1s = np.array(am1s)
am2s = np.array(am2s)
ad1s = np.array(ad1s)
ad2s = np.array(ad2s)
adms = np.array(adms)
addms = np.array(addms)

plt.title("Z distribution")
plt.hist(zs, bins=100)
plt.show()

plt.title("Theta distribution")
plt.hist(thetas, bins=100)
plt.show()

plt.title("Phi distribution")
plt.hist(phis, bins=100)
plt.show()

#now check accuracy of rotations
plt.title("Alpha vs angle(mother, 1)")
plt.hist(alphas-am1s, bins=100)
plt.xlabel("alpha - angle(mother, 1)")
plt.show()

plt.title("Beta vs angle(mother, 2)")
plt.hist(thetas-alphas-am2s, bins=100)
plt.xlabel("beta - angle(mother, 2)")
plt.show()

plt.title("Theta vs angle(1, 2)")
plt.hist(thetas-a12s, bins=100)
plt.xlabel("theta - angle(1, 2)")
plt.show()

plt.title("Phi vs angle(dipole, mother dipole)")
phis[phis>np.pi] = 2*np.pi - phis[phis>np.pi]
plt.hist((phis - addms), bins=100)
plt.xlabel("phi - angle(dipole, mother dipole)")
plt.show()

plt.title("Angles w.r.t. dipole")
plt.hist([ad1s, ad2s, adms], bins=100, 
         label = ['angle(1, dipole)', 'angle(2, dipole)', 'angle(mother, dipole)'],
         histtype='step')
plt.legend()
plt.show()
