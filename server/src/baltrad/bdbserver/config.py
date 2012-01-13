import jprops

_undefined = object()

class Error(Exception):
    """base configuration error"""

class PropertyLookupError(Error):
    """property not found
    """

class PropertyValueError(Error):
    """invalid property value
    """

class Properties(object):
    def __init__(self, values, prefix=""):
        """
        :param values: initial set of values
        :type values: :class:`dict`
        :prefix: the prefix to use for keys when getting values from this
                 instance
        :type prefix: :class:`str`
        """
        self._values = values
        self._prefix = prefix
    
    @property
    def prefix(self):
        return self._prefix
    
    def get(self, key, default=_undefined):
        """get the value associated with the key

        :param key: the key for which to look up the value
        :param default: default value if the key is not found
        :raise: :class:`PropertyLookupError` if the key is not found
                and no default value is provided.
        """
        try:
            return self[key]
        except PropertyLookupError:
            if default != _undefined:
                return default
            else:
                raise
    
    def get_list(self, key, default=_undefined, sep=" "):
        """get list of values associated with the key

        :param key: the key for which to look up the value
        :param default: default value if the key is not found. This can be
                        provided either as a `str` or a `list`. If `str`, the
                        value goes through the splitting, otherwise it is
                        returned as it is.
        :param sep: value separator
        :raise: :class:`PropertyLookupError` if the key is not found
                and no default value is provided.

        """
        value = self.get(key, default)
        if not isinstance(value, basestring):
            return value
        
        return [part.strip() for part in value.split(sep)]
    
    def get_boolean(self, key, default=_undefined):
        """get boolean value associated with the key

        """
        value = self.get(key, default)
        if value in ("True", "true", "yes", "on", "1", True):
            return True
        elif value in ("False", "false", "no", "off", "0", False):
            return False
        else:
            raise PropertyValueError("can't parse boolean from: %s" % value)
    
    def __getitem__(self, key):
        """get the value associated with the key

        :param key: the key for which to look up the value
        :raise: :class:`PropertyLookupError` if the key is not found
                and no default value is provided.
        """
        full_key = self._prefix + key
        try:
            return self._values[full_key]
        except LookupError:
            raise PropertyLookupError(full_key)
    
    def filter(self, prefix):
        """apply a prefix to the key lookups

        :param prefix: the prefix to apply
        :return: a new :class:`Properties` instance with the specified prefix
        """
        return Properties(self._values, prefix=self._prefix + prefix)
    
    def get_keys(self):
        """get available keys

        :return: list of keys matching for current prefix
        """
        result = []
        for key in self._values:
            if key.startswith(self._prefix):
                result.append(key[len(self._prefix):])
        return result
            
    
    @classmethod
    def load(cls, path):
        with open(path) as fp:
           return Properties(jprops.load_properties(fp))
