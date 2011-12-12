import jprops

_undefined = object()

class Properties(object):
    def __init__(self, values, prefix=""):
        self._values = values
        self._prefix = prefix
    
    def get(self, key, default=_undefined):
        try:
            return self[key]
        except LookupError:
            if default != _undefined:
                return default
            else:
                raise
    
    def get_boolean(self, key, default=_undefined):
        value = self.get(key, default)
        if value in ("True", "true", "yes", "on", "1", True):
            return True
        elif value in ("False", "false", "no", "off", "0", False):
            return False
        else:
            raise ValueError("can't parse boolean from: %s" % value)
    
    def __getitem__(self, key):
        return self._values[self._prefix + key]
    
    def filter(self, prefix):
        return Properties(self._values, prefix=self._prefix + prefix)
    
    @classmethod
    def load(cls, path):
        with open(path) as fp:
           return Properties(jprops.load_properties(fp))
