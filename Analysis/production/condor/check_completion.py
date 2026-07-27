import argparse

parser = argparse.ArgumentParser(description='Check completion of production')
parser.add_argument('taskid', type=str, help='Task ID')
parser.add_argument('--destination_base', type=str, default='/store/group/lpcpfnano/srothman/condor_production')
parser.add_argument('--gateway', type=str, default='cmseos.fnal.gov')
args = parser.parse_args()

taskid = args.taskid

import os
import os.path as osp
import sys
import fsspec_xrootd
import numpy as np

with open("working/%s/filelist.txt"%taskid, 'r') as f:
    lines = f.readlines()
    N = len(lines)

#taskid has form *_MONTH_DAY_YEAR
date = '_'.join(taskid.split('_')[-3:])
taskname = '_'.join(taskid.split('_')[:-3])

fs = fsspec_xrootd.XRootDFileSystem(hostid=args.gateway)
destination = osp.join(args.destination_base, date, taskname)

if not fs.exists(destination):
    print('destination does not exist')
    sys.exit(1)

files = fs.ls(destination)

done_selected = np.asarray([False for i in range(N)])
done_dropped = np.asarray([False for i in range(N)])

for file in files:
    if not file['name'].endswith('.root'):
        print("WARNING: NON-ROOT FILE", file['name'])
        continue

    if file['size'] == 0:
        print("WARNING: EMPTY FILE", file['name'])
        print("Delete? (y/n)")
        response = input()
        if response == 'y':
            fs.rm(file['name'])
            print("Deleted")
        else:
            print("Not deleted")
        print()
        continue

    basename = osp.basename(file['name'])

    if basename.startswith("NANO_selected"):
        index = int(basename.split('_')[-1].split('.')[0])
        done_selected[index] = True
    elif basename.startswith("NANO_dropped"):
        index = int(basename.split('_')[-1].split('.')[0])
        done_dropped[index] = True
    else:
        print("WARNING: UNKNOWN FILE", file['name'])
        print("Delete? (y/n)")
        response = input()
        if response == 'y':
            fs.rm(file['name'])
            print("Deleted")
        else:
            print("Not deleted")

for i in range(N):
    if done_selected[i] and not done_dropped[i]:
        print("Selected file %d exists, but dropped file %d is missing" % (i, i))
        print("Please investigate why")
        print("Delete errant selected file? (y/n)")
        response = input()
        if response == 'y':
            fs.rm(osp.join(destination, f'NANO_selected_{i}.root'))
            print("Deleted")
            done_selected[i] = False
        else:
            print("Not deleted")
        print()
    elif not done_selected[i] and done_dropped[i]:
        print("Dropped file %d exists, but selected file %d is missing" % (i, i))
        print("Please investigate why")
        print("Delete errant dropped file? (y/n)")
        response = input()
        if response == 'y':
            fs.rm(osp.join(destination, f'NANO_dropped_{i}.root'))
            print("Deleted")
            done_dropped[i] = False
        else:
            print("Not deleted")
        print()
    #elif not done_selected[i] and not done_dropped[i]:
    #    print("Neither file %d exists" % i)
    #    print("Please investigate why")
    #    print()

completed = np.sum(done_selected & done_dropped)
print("Completed %d/%d files (%0.2f%%)" % (completed, N, 100*completed/(N)))
#print("Missing files indices are:")
#for i in range(N):
#    if not done_selected[i] or not done_dropped[i]:
#        print('\t',i)
print("Setup retry job? (y/n)")
response = input()
if response == 'y':
    #os.system(f'python setup_retry.py {taskid}')
    print("Retry job setup")
    import shutil
    shutil.copyfile('templates/condor.sub', 'working/%s/retry.sub' % taskid)
    index_str = ', '.join([str(i) for i in range(N) if not (done_selected[i]&done_dropped[i])])
    os.system("sed -i 's/INDEXEXPRESSION/in %s/g' working/%s/retry.sub" % (index_str, taskid))
