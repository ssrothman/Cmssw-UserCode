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
for line in lines[8:]:
    splitted = line.split(',\t')
    zs.append(float(splitted[0]))
    thetas.append(float(splitted[1]))
    phis.append(float(splitted[2]))

plt.title("Z distribution")
plt.hist(zs, bins=100)
plt.show()

plt.title("Theta distribution")
plt.hist(thetas, bins=100)
plt.show()

plt.title("Phi distribution")
plt.hist(phis, bins=100)
plt.show()
