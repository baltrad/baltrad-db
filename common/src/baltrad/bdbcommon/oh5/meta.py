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
import sys
from xml.etree import ElementTree
import logging

from . node import Attribute, Group, NodeIterator
from . io import HlHdfMetadataReader

from collections.abc import MutableMapping, Mapping

logger = logging.getLogger("baltrad.bdbcommon")

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
        group_path, attr_name = os.path.split(self._path)

        group_node = obj.find_node(group_path)
        if not group_node:
            parent_path, group_name = os.path.split(group_path)
            group_node = obj.add_node(parent_path, Group(group_name))
        
        attr_node = obj.find_node(self._path)
        if not attr_node:
            obj.add_node(group_path, Attribute(attr_name, value))
        else:
            attr_node.value = value

class Metadata(object):
    what_object = AttributeProperty("/what/object")
    what_date = AttributeProperty("/what/date", Attribute.value_date)
    what_time = AttributeProperty("/what/time", Attribute.value_time)
    what_source = AttributeProperty("/what/source")
    bdb_uuid = AttributeProperty("/_bdb/uuid")
    bdb_metadata_hash = AttributeProperty("/_bdb/metadata_hash")
    bdb_file_size = AttributeProperty("/_bdb/file_size")
    bdb_source = AttributeProperty("/_bdb/source")
    bdb_source_name = AttributeProperty("/_bdb/source_name")
    bdb_stored_date = AttributeProperty("/_bdb/stored_date", Attribute.value_date)
    bdb_stored_time = AttributeProperty("/_bdb/stored_time", Attribute.value_time)

    def __init__(self):
        self._root = Group("")
        
    def add_node(self, parent_path, node):
        """add a node

        :param parent_path: path of the parent node
        :param node: the node to add
        :raise: `LookupError` if the parent node is not found
        """
        parent = self.node(parent_path)
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
        return [node.json_repr() for node in self.iternodes()]

class Source(MutableMapping):
    def __init__(self, name=None, values={}, parent=None):
        self.name = name
        self._values = dict(values)
        self.parent = parent

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

    def has_key(self, k):
        return k in self._values

    def __eq__(self, other):
        if isinstance(other, Source):
            return self.name == other.name and self._values == other._values and self.parent == other.parent
        elif isinstance(other, Mapping):
            return self._values == other
        return False
    
    def __repr__(self):
        return "Source(%r, parent=%r, values=%r)" % (self.name, self.parent, self._values)
    
    def __str__(self):
        return self.to_string()

    def to_string(self):
        return ",".join([":".join(kv) for kv in sorted(self._values.items())])
    
    @classmethod
    def from_string(cls, value):
        result = Source()
        for item in value.split(","):
            k, v = item.split(":")
            result[k] = v
        return result
    
    @classmethod
    def _create_from_etree_element(cls, element):
        result = Source(element.tag)
        for key, value in element.items():
            result[key.upper()] = value
        return result

    
    @classmethod
    def from_rave_xml(cls, xml):
        """read sources from rave configuration
        """
        radar_db = ElementTree.XML(xml)
        result = []
        for org_def in radar_db:
            org_source = cls._create_from_etree_element(org_def)
            result.append(org_source)
            for radar_def in org_def:
                source = cls._create_from_etree_element(radar_def)
                source["NOD"] = radar_def.tag
                source.parent = org_source.name
                result.append(source)
        return result
