# Copyright 2010-2011 Estonian Meteorological and Hydrological Institute
# 
# This file is part of baltrad-db.
# 
# baltrad-db is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# baltrad-db is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.

import collections
import os.path

import _pyhl as pyhl

from . node import Attribute, Group, Dataset, NodeIterator

class AttributeProperty(object):
    def __init__(self, path, fget=None):
        self._path = path
        if fget:
            self._fget = fget
        else:
            self._fget = lambda attr: attr.value

    def __get__(self, obj, objtype=None):
        if obj is None:
            return self
        node = obj.find_node(self._path)
        if not node:
            return None
        else:
            return self._fget(node)
    
    def __set__(self, obj, value):
        #XXX: this could be more generic and efficient
        grouppath, name = os.path.split(self._path)
        groupnode = obj.find_node(grouppath)
        if not groupnode:
            groupnode = obj.add_group(grouppath)
        attrnode = obj.find_node(self._path)
        if not attrnode:
            obj.add_attribute(self._path, value)
        else:
            attrnode.value = value

class Metadata(object):
    what_object = AttributeProperty("/what/object")
    what_date = AttributeProperty("/what/date", Attribute.value_date)
    what_time = AttributeProperty("/what/time", Attribute.value_time)
    what_source = AttributeProperty("/what/source")
    bdb_uuid = AttributeProperty("/_bdb/uuid")
    bdb_metadata_hash = AttributeProperty("/_bdb/metadata_hash")
    bdb_file_size = AttributeProperty("/_bdb/file_size")
    bdb_source = AttributeProperty("/_bdb/source")
    bdb_stored_date = AttributeProperty("/_bdb/stored_date", Attribute.value_date)
    bdb_stored_time = AttributeProperty("/_bdb/stored_time", Attribute.value_time)

    def __init__(self):
        self._root = Group("")
    
    def add_group(self, path):
        return self._add_node(path, Group(""))
    
    def add_attribute(self, path, value):
        return self._add_node(path, Attribute("", value))
    
    def add_dataset(self, path):
        return self._add_node(path, Dataset(""))
    
    def _add_node(self, path, node):
        basepath, name = os.path.split(path)
        node.name = name
        parent = self.node(basepath)
        return parent.add_child(node)
    
    def root(self):
        return self._root
    
    def node(self, path):
        n = self.find_node(path)
        if not n:
            raise LookupError(path)
        return n

    def find_node(self, path):
        path = os.path.normpath(path)
        if path == "/":
            return self.root()

        names = path.split("/")
        names.pop(0)
        node = self.root()
        for name in names:
            try:
                node = node.child_by_name(name)
            except LookupError:
                return None
        return node
    
    def iternodes(self):
        return NodeIterator(self.root())
             
    def source(self):
        return Source.from_string(self.what_source)
        
    @staticmethod
    def from_file(filepath):
        reader = HlHdfMetadataReader()
        return reader.read(filepath)

    def json_repr(self):
        return self.root().json_repr()

class HlHdfMetadataReader(object):
    def __init__(self):
        pass
    
    def read(self, filepath):
        nodelist = pyhl.read_nodelist(filepath)
        nodelist.selectMetadata()
        nodelist.fetch()

        nodes = nodelist.getNodeNames()
        
        nodenames = sorted(nodes.keys())

        meta = Metadata()
        for name in nodenames:
            node_type = nodes[name]
            if node_type == pyhl.ATTRIBUTE_ID:
                meta.add_attribute(name, nodelist.getNode(name).data())
            elif node_type == pyhl.GROUP_ID:
                meta.add_group(name)
            elif node_type == pyhl.DATASET_ID:
                meta.add_dataset(name)
            else:
                raise RuntimeError("unhandled hlhdf node type: %s" % node_type)
        return meta

class HlHdfMetadataWriter(object):
    def __init__(self):
        pass
    
    def write(self, metadata, filepath):
        hlnodes = pyhl.nodelist()

        iterator = metadata.iternodes()
        iterator.next() # skip root
        for node in iterator:
            hlnodes.addNode(self._create_hlnode(node))
        hlnodes.write(filepath)
    
    def _create_hlnode(self, node):
        if isinstance(node, Group):
            return pyhl.node(pyhl.GROUP_ID, node.path())
        elif isinstance(node, Attribute):
            hlnode = pyhl.node(pyhl.ATTRIBUTE_ID, node.path())
            hlnode.setScalarValue(-1, node.value, self._get_hl_attribute_type(node.value), -1)
            return hlnode
        else:
            raise RuntimeError("unhandled node type: %s" % node.type_id)
    
    def _get_hl_attribute_type(self, value):
        if isinstance(value, int):
            return "int"
        elif isinstance(value, str):
            return "string"
        elif isinstance(value, float):
            return "double"
        else:
            raise RuntimeError("unhandled attribute value type: %s" % type(value))

class Source(collections.MutableMapping):
    def __init__(self, default={}):
        self._values = dict(default)
    
    def __getitem__(self, k):
        return self._values[k]
    
    def __setitem__(self, k, v):
        self._values[k] = v
    
    def __delitem__(self, k):
        del self._values[k]
    
    def __len__(self):
        return len(self._values)
    
    def __iter__(self):
        return iter(self._values)
    
    name = property(
        lambda self: self._values.get("_name"),
        lambda self, value: self._values.update({"_name": value})
    )

    def to_string(self, include_hidden=False):
        items = []
        for k in sorted(self._values.keys()):
            if k.startswith("_") and not include_hidden:
                continue
            items.append(k + ":" + self._values[k])
        return ",".join(items)
    
    @staticmethod
    def from_string(value):
        result = Source()
        for item in value.split(","):
            k, v = item.split(":")
            result[k] = v
        return result
