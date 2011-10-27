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

from nose.tools import eq_, raises

from baltrad.bdb.oh5.node import Attribute, Group, Node, NodeJsonEncoder

class TestNode(object):
    def test_path_no_parent(self):
        node = Node("name")
        eq_("/name", node.path())
    
    def test_path(self):
        a = Node("a")
        b = Node("b")
        a.add_child(b)
        eq_("/a/b", b.path())

       
class TestAttribute(object):
    def test_ctor_date_value(self):
        attr = Attribute("attr", datetime.date(2000, 1, 2))
        eq_("20000102", attr.value)
    
    def test_ctor_time_value(self):
        attr = Attribute("attr", datetime.time(13, 1, 2))
        eq_("130102", attr.value)
    
    def test_set_value_date(self):
        attr = Attribute("attr")
        attr.value = datetime.datetime(2000, 1, 2)
        eq_("20000102", attr.value)
    
    def test_set_value_time(self):
        attr = Attribute("attr")
        attr.value = datetime.time(13, 14, 15)
        eq_("131415", attr.value)
    
    def test_value_date(self):
        attr = Attribute("attr", "20000102")
        eq_(datetime.date(2000, 1, 2), attr.value_date())
    
    @raises(ValueError)
    def test_value_date_invalid(self):
        attr = Attribute("attr", "asd")
        attr.value_date()

    def test_value_time(self):
        attr = Attribute("attr", "130102")
        eq_(datetime.time(13, 1, 2), attr.value_time())
        
    @raises(ValueError)
    def test_value_time_invalid(self):
        attr = Attribute("attr", "asd")
        attr.value_time()

class TestNodeJsonEncoder(object):
    def setUp(self):
        self.encoder = NodeJsonEncoder(indent=None, sort_keys=True)
    
    def test_group(self):
        a = Group("a")

        expected = "".join([
            '{',
              '"children": [], ',
              '"name": "a", ',
              '"type": "group"',
            '}'
        ])

        eq_(expected, self.encoder.encode(a))
    
    def test_attribute(self):
        a = Attribute("a", 1)
        expected = "".join([
            '{',
              '"name": "a", ',
              '"type": "attribute", ',
              '"value": 1',
            '}'
        ])
        eq_(expected, self.encoder.encode(a))
   
