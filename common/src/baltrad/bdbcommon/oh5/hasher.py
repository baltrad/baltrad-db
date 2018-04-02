import hashlib
import sys
from . node import Attribute

encoding_necessary=False
if sys.version_info >= (3,):
  encoding_necessary=True

class MetadataHasher(object):
  
    def hash(self, metadata):
        attribute_strings = []
        for node in metadata.iternodes():
            if (isinstance(node, Attribute)):
                attribute_strings.append(self.attribute_string(node))
        attribute_strings.sort()
        hashfunc = hashlib.sha1()
        for s in attribute_strings:
            if encoding_necessary:
              s = s.encode('utf-8')
            hashfunc.update(s)
        return hashfunc.hexdigest()
    
    def attribute_string(self, attr):
        return attr.path() + "=" + attr.value_str()
