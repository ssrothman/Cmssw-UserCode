import argparse

parser = argparse.ArgumentParser(description='Merge files')
parser.add_argument('taskid', type=str, help='Task ID')
parser.add_argument('--destination_base', type=str, default='/store/group/lpcpfnano/srothman')
parser.add_argument('--gateway', type=str, default='cmseos.fnal.gov')
parser.add_argument('--output', type=str, default='merged')
args = parser.parse_args()

taskid = args.taskid

import os
import os.path as osp
import sys
import fsspec_xrootd
import numpy as np

fs = fsspec_xrootd.XRootDFileSystem(hostid=args.gateway)
destination = osp.join(args.destination_base, taskid)

if not fs.exists(destination):
    print('destination does not exist')
    sys.exit(1)

files = fs.ls(destination)

selected_files = []
dropped_files = []

for file in files:
    if file['size'] == 0:
        continue;

    if not file['name'].endswith('.root'):
        continue

    basename = osp.basename(file['name'])
    if basename.startswith("NANO_selected"):
        selected_files.append('root://' + args.gateway + '/' + file['name'])

    if basename.startswith("NANO_dropped"):
        dropped_files.append('root://' + args.gateway + '/' + file['name'])

if len(selected_files) == 0:
    print("No selected files found")
    sys.exit(1)

if len(dropped_files) == 0:
    print("No dropped files found")
    sys.exit(1)

selected_files = selected_files[:]

print("Merging selected files...")
#destination_selected = 'root://' + args.gateway + '/' + osp.join(destination, args.output + '_selected.root')
destination_selected = 'test.root'
#print('hadd %s ' % destination_selected + ' '.join(selected_files))
os.system('hadd -fk -j %s ' % destination_selected + ' '.join(selected_files))
print("DESTINATION", destination_selected)
