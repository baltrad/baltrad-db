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

import datetime
import json
import os.path
import weakref

class Node(object):
    """Base node

    :param name: name of this node
    :type node: string
    :param parent: parent node
    :type parent: `Node`
    """
    
    type_id = None

    def __init__(self, name):
        """constructor
        
        :param name: name of this node
        """
        self.parent = None
        self.name = name
        self._children = {}
    
    def path(self):
        """absolute path of this node
        """
        parent_path = "/"
        if self.parent:
            parent_path = self.parent.path()
        return os.path.join(parent_path, self.name)
    
    def add_child(self, child):
        """add a child to this node

        :param child: the child to add
        :type child: `Node`
        """
        self._children[child.name] = child
        child.parent = self
        return child
    
    def has_child_by_name(self, name):
        return name in self._children
    
    def child_by_name(self, name):
        return self._children[name]
    
    def children(self):
        """list of this node's children
        """
        return self._children.values()
    
    def _get_parent(self):
        if self._parent:
            return self._parent()

    def _set_parent(self, parent):
        """set parent node
        """
        self._parent = weakref.ref(parent) if parent else None

    parent = property(_get_parent, _set_parent)

    def json_repr(self):
        return {
            "name": self.name,
            "children": [child.json_repr() for child in self.children()],
        }

class Group(Node):
    """Group node
    """

    type_id = 1
    
    def json_repr(self):
        repr_ = super(Group, self).json_repr()
        repr_["type"] = "group"
        return repr_
    
class Attribute(Node):
    type_id = 2

    def __init__(self, name, value=None):
        Node.__init__(self, name)
        self.value = value
     
    def set_value(self, value):
        if isinstance(value, datetime.date):
            value = value.strftime("%Y%m%d")
        elif isinstance(value, datetime.time):
            value = value.strftime("%H%M%S")
        self._value = value
    
    value = property(
        lambda self: self._value,
        set_value
    )
    
    def value_str(self):
        return str(self.value)
        
    def value_time(self):
        return datetime.datetime.strptime(self.value, "%H%M%S").time()
    
    def value_date(self):
        return datetime.datetime.strptime(self.value, "%Y%m%d").date()
    
    def json_repr(self):
        return {
            "type": "attribute",
            "name": self.name,
            "value": self.value,
        }

class Dataset(Node):
    type_id = 3

    def json_repr(self):
        repr_ = super(Dataset, self).json_repr()
        repr_["type"] = "dataset"
        return repr_

class NodeIterator(object):
    def __init__(self, node=None):
        self._nodes = [node]
    
    def __iter__(self):
        return self
    
    def next(self):
        try:
            node = self._nodes.pop(0)
        except IndexError:
            raise StopIteration()
        
        if node.children:
            self._nodes.extend(node.children())
        return node
