import json
import io

with io.open("Analysis/python/config/config.json", 'r') as f:
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
