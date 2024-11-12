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
    paths = fs.ls(base_dirs[key])
    for path in paths:
        if path['type'] == 'file' and path['size'] > 0 and path['name'].endswith('.root'):
            filelist.append('root://'+gateway+'/'+path['name'])
        else:
            print("erronous path:", path)

    print("\tfound %d rootfiles" % len(filelist))
    with open(key + "_filelist.txt", 'w') as f:
        for item in filelist:
            f.write("%s\n" % item)
