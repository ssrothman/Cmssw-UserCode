import FWCore.ParameterSet.Config as cms

def pyval_to_cmsval(value):
    if isinstance(value, dict):
        result = cms.PSet()
        for key, val in value.items():
            result.__setattr__(key, pyval_to_cmsval(val))
        return result
    if isinstance(value, bool):
        return cms.bool(value)
    elif isinstance(value, int):
        return cms.int32(value)
    elif isinstance(value, float):
        return cms.double(value)
    elif isinstance(value, str):
        if value.endswith('.json'):
            print("Interpreting string value as file path: %s" % value)
            # Assume this is a file path and convert to cms.FileInPath
            return cms.FileInPath(value)
        else:
            return cms.string(value)
    elif isinstance(value, list):
        if all(isinstance(v, bool) for v in value):
            return cms.vbool(value)
        elif all(isinstance(v, int) for v in value):
            return cms.vint32(value)
        elif all(isinstance(v, float) for v in value):
            return cms.vdouble(value)
        elif all(isinstance(v, str) for v in value):
            return cms.vstring(value)
        else:
            raise ValueError("Unsupported list element type in parameter: %s" % value)
    else:
        raise ValueError("Unsupported parameter type: %s" % type(value))

