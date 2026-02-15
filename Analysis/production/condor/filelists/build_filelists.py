import fsspec_xrootd
import json

with open("base_dirs.json", 'r') as f:
    config = json.load(f)

gateway = config["gateway"]
base_dirs = config['base_dirs']

fs = fsspec_xrootd.XRootDFileSystem(hostid = gateway)

for key in base_dirs:
    filelist = []
    print("Building filelist for", key)
    if type(base_dirs[key]) == str:
        basepath = base_dirs[key]
        minindex = None
        maxindex = None
    elif type(base_dirs[key]) == dict:
        basepath = base_dirs[key]['path']

        if 'minindex' in base_dirs[key]:
            minindex = base_dirs[key]['minindex']
        else:           
            minindex = None

        if 'maxindex' in base_dirs[key]:
            maxindex = base_dirs[key]['maxindex']
        else:
            maxindex = None

    paths = fs.ls(basepath)
    for path in paths:
        if path['type'] == 'file' and path['size'] > 0 and path['name'].endswith('.root'):
            if minindex is not None or maxindex is not None:
                fname = path['name'].split('/')[-1]
                #filenames end _{index}.root
                index = int(fname.split('_')[-1].split('.root')[0])
                if minindex is not None and index < minindex:
                    continue
                if maxindex is not None and index >= maxindex:
                    continue

            filelist.append('root://'+gateway+'/'+path['name'])
        else:
            print("erronous path:", path)

    print("\tfound %d rootfiles" % len(filelist))
    with open(key + "_filelist.txt", 'w') as f:
        for item in filelist:
            f.write("%s\n" % item)
