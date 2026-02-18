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

import pytest

from baltrad.bdbcommon.oh5.node import Attribute, Group, Dataset, Node

class TestNode(object):
    def test_path_no_parent(self):
        node = Node("name")
        assert(node.path() == "/name")
    
    def test_path(self):
        a = Node("a")
        b = Node("b")
        a.add_child(b)
        assert(b.path() == "/a/b")

       
class TestAttribute(object):
    def test_ctor_date_value(self):
        attr = Attribute("attr", datetime.date(2000, 1, 2))
        assert(attr.value == "20000102")
    
    def test_ctor_time_value(self):
        attr = Attribute("attr", datetime.time(13, 1, 2))
        assert(attr.value == "130102")
    
    def test_set_value_date(self):
        attr = Attribute("attr")
        attr.value = datetime.datetime(2000, 1, 2)
        assert(attr.value == "20000102")
    
    def test_set_value_time(self):
        attr = Attribute("attr")
        attr.value = datetime.time(13, 14, 15)
        assert(attr.value == "131415")
    
    def test_value_date(self):
        attr = Attribute("attr", "20000102")
        assert(attr.value_date() == datetime.date(2000, 1, 2))
    
    def test_value_date_invalid(self):
        attr = Attribute("attr", "asd")
        with pytest.raises(ValueError):
            attr.value_date()

    def test_value_time(self):
        attr = Attribute("attr", "130102")
        assert(attr.value_time() == datetime.time(13, 1, 2))
        
    def test_value_time_invalid(self):
        attr = Attribute("attr", "asd")
        with pytest.raises(ValueError):
            attr.value_time()

class TestNodeJsonRepr(object):
    def test_group(self):
        a = Group("a")

        expected = {
            "path": "/a",
            "type": "group",
        }
        assert(a.json_repr() == expected)
    
    def test_attribute(self):
        a = Attribute("a", 1)
        expected = {
            "path": "/a",
            "type": "attribute",
            "value": 1
        }
        assert(a.json_repr() == expected)
    
    def test_dataset(self):
        a = Dataset("a")

        expected = {
            "path": "/a",
            "type": "dataset",
        }
        assert(a.json_repr() == expected)
