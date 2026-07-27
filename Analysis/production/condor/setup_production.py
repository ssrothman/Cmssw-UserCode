import argparse

parser = argparse.ArgumentParser(description='Setup production environment')
parser.add_argument('cmsRun', type=str, help='Path to cmsRun config')
parser.add_argument('dataset', type=str, help='Dataset name')
parser.add_argument('--destination_base', type=str, default='root://cmseos.fnal.gov//store/group/lpcpfnano/srothman/condor_production')
parser.add_argument("--name-extra', type=str, default='', help='Extra name to append to the output directory'", dest='name_extra')
parser.add_argument('--submit', action='store_true', help='Submit jobs after setting up')
parser.add_argument('--no-submit', action='store_false', help='Do not submit jobs after setting up')
args = parser.parse_args()

print(f'cmsRun: {args.cmsRun}')
print(f'dataset: {args.dataset}')
print(f'destination_base: {args.destination_base}')
print(f'name_extra: {args.name_extra}')

import os
import os.path as osp
import shutil
import datetime

now = datetime.datetime.now()
nowstr = now.strftime("%b_%d_%Y")


if args.name_extra:
    configname = args.dataset + '_' + args.name_extra + '_' + nowstr
else:
    configname = args.dataset + '_' + nowstr

if args.name_extra:
    thedestination = osp.join(args.destination_base, nowstr, args.dataset + '_' + args.name_extra)
else:
    thedestination = osp.join(args.destination_base, nowstr, args.dataset)
print(f'destination: {thedestination}')

# Create working directory
print("Setting up config")
os.makedirs('working/%s' % configname, exist_ok=False)
os.makedirs('working/%s/output' % configname, exist_ok=False)
#copy the cmsRun config to the working directory
shutil.copyfile(args.cmsRun, 'working/%s/run.py' % configname)
shutil.copyfile('filelists/%s_filelist.txt' % args.dataset, 'working/%s/filelist.txt' % configname)
shutil.copyfile('templates/condor.sub', 'working/%s/condor.sub' % configname)
shutil.copyfile('templates/CONDOR_EXEC.sh', 'working/%s/CONDOR_EXEC.sh' % configname)

os.system("sed -i 's/CONFIGNAME/%s/g' working/%s/CONDOR_EXEC.sh" % (configname, configname))
destination_for_sed = thedestination.replace("/", "\/")
os.system("sed -i 's/DESTINATION/%s/g' working/%s/CONDOR_EXEC.sh" % (destination_for_sed, configname))
os.system("sed -i 's/INDEXEXPRESSION/from seq 0 %d |/g' working/%s/condor.sub" % (sum(1 for line in open('filelists/%s_filelist.txt' % args.dataset))-1, configname))

print("Making tarball")
os.chdir('working/%s' % configname)
os.system('tar -czf %s.tar.gz *' % configname)
print("Copying tarball to EOS")
os.system('xrdcp -f %s.tar.gz root://cmseos.fnal.gov//store/user/srothman/' % configname)

if args.submit:
    os.system('condor_submit condor.sub')
    print("Jobs submitted")
elif args.no_submit:
    print("Jobs not submitted")
else:
    print("Submit jobs?")
    submit = input("y/n: ")
    if submit == 'y':
        os.system('condor_submit condor.sub')
        print("Jobs submitted")
