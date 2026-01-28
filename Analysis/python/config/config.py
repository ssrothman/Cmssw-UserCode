import json
import io
import os

config = None

def SETUP_CONFIG(configname):
    CMSSW_BASE = os.environ['CMSSW_BASE']
    print("CMSSW_BASE: %s" % CMSSW_BASE)
 
    global config

    with io.open("%s/src/SRothman/Analysis/python/config/%s.json"%(CMSSW_BASE, configname), 'r') as f:
        config = json.load(f)

    def encode_level(config):
        if type(config) is dict:
            for key in config:
                config[key] = encode_level(config[key])
        elif type(config) is list:
            for i in range(len(config)):
                config[i] = encode_level(config[i])
        elif type(config) is unicode:
            config = config.encode('ascii')
        elif type(config) in [int, float, bool, str]:
            pass
        else:
            raise TypeError("Unknown type: %s" % type(config))
        return config

    config = encode_level(config)

def load_config(configname):
    CMSSW_BASE = os.environ['CMSSW_BASE']
    print("CMSSW_BASE: %s" % CMSSW_BASE)
 
    with io.open("%s/src/SRothman/Analysis/python/config/%s.json"%(CMSSW_BASE, configname), 'r') as f:
        cfg= json.load(f)
    
    def encode_level(config):
        if type(config) is dict:
            for key in config:
                config[key] = encode_level(config[key])
        elif type(config) is list:
            for i in range(len(config)):
                config[i] = encode_level(config[i])
        elif type(config) is unicode:
            config = config.encode('ascii')
        elif type(config) in [int, float, bool, str]:
            pass
        else:
            raise TypeError("Unknown type: %s" % type(config))
        return config

    cfg = encode_level(cfg)
    return cfg