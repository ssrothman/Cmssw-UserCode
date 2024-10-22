import json
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('filelist', type=str, help='filelist')
parser.add_argument('num_jobs', type=int, help='number of jobs')
parser.add_argument('output', type=str, help='output json file')
args = parser.parse_args()

with open(args.filelist) as f:
    files = f.readlines()

cleanfiles = []
for file in files:
    clean = file.strip()
    if clean:
        cleanfiles.append(clean)

jobdict = {i : [] for i in range(args.num_jobs)}
for i, file in enumerate(cleanfiles):
    jobdict[i % args.num_jobs].append(file)

with open(args.output, 'w') as f:
    json.dump(jobdict, f, indent=4)
