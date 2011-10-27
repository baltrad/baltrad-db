import hashlib

from . node import Attribute

class MetadataHasher(object):
    def __init__(self):
        self._ignore = ["what/source"]
    
    def hash(self, metadata):
        attribute_strings = []
        for node in metadata.iternodes():
            if (isinstance(node, Attribute)):
                if (not self.is_ignored(node.path())):
                    attribute_strings.append(self.attribute_string(node))
        attribute_strings.sort()
        hashfunc = hashlib.sha1()
        for string in attribute_strings:
            hashfunc.update(string)
        return hashfunc.hexdigest()
    
    def attribute_string(self, attr):
        return attr.path() + "=" + attr.value_str()
    
    def is_ignored(self, path):
        for pattern in self._ignore:
            if path.find(pattern) != -1:
                return True
        return False
