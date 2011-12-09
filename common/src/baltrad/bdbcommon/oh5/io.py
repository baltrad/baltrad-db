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

import _pyhl as pyhl

from . node import Attribute, Group

class HlHdfMetadataReader(object):
    def __init__(self):
        pass
    
    def read(self, filepath):
        nodelist = pyhl.read_nodelist(filepath)
        nodelist.selectMetadata()
        nodelist.fetch()

        nodes = nodelist.getNodeNames()
        
        nodenames = sorted(nodes.keys())

        from . meta import Metadata

        metadata = Metadata()
        for name in nodenames:
            node_type = nodes[name]
            if node_type == pyhl.ATTRIBUTE_ID:
                metadata.add_attribute(name, nodelist.getNode(name).data())
            elif node_type == pyhl.GROUP_ID:
                metadata.add_group(name)
            elif node_type == pyhl.DATASET_ID:
                metadata.add_dataset(name)
            else:
                raise RuntimeError("unhandled hlhdf node type: %s" % node_type)
        return metadata

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

