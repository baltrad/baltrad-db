import hashlib

from . node import Attribute

class MetadataHasher(object):
  
    def hash(self, metadata):
        attribute_strings = []
        for node in metadata.iternodes():
            if (isinstance(node, Attribute)):
                attribute_strings.append(self.attribute_string(node))
        attribute_strings.sort()
        hashfunc = hashlib.sha1()
        for string in attribute_strings:
            string = string.encode('utf-8')
            hashfunc.update(string)
        return hashfunc.hexdigest()
    
    def attribute_string(self, attr):
        return attr.path() + "=" + attr.value_str()
