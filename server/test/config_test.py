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

from nose.tools import eq_, ok_, raises

from baltrad.bdbserver import config

class TestProperties(object):
    def setup(self):
        self.properties = config.Properties({
            "foo.bar.baz": 1,
            "foo.bar.qwe": 2,
            "bar.foo.baz": "qwe",
            "qwe": "true",
            "asd": "false",
            "list": "a, b, c",
        })
    
    def test_get(self):
        eq_(1, self.properties.get("foo.bar.baz"))
    
    @raises(config.PropertyLookupError)
    def test_get_missing(self):
        self.properties.get("qwe.asd")
    
    def test_get_missing_default(self):
        eq_(2, self.properties.get("qwe.asd", 2))
    
    def test_getitem(self):
        eq_(2, self.properties["foo.bar.qwe"])
    
    @raises(config.PropertyLookupError)
    def test_getitem_missing(self):
        self.properties["qwe.asd"]
    
    def test_getboolean(self):
        eq_(True, self.properties.get_boolean("qwe"))
        eq_(False, self.properties.get_boolean("asd"))
    
    @raises(config.PropertyValueError)
    def test_getboolean_invalid_value(self):
        self.properties.get_boolean("bar.foo.baz")

    def test_get_list(self):
        eq_(["a", "b", "c"], self.properties.get_list("list", sep=","))
    
    def test_get_list_default_list(self):
        default = [1, 2, 3]
        eq_(default, self.properties.get_list("nxlist", default=default))
    
    def test_get_list_default_str(self):
        result = self.properties.get_list("nxlist", default="1 2 3")
        eq_(["1", "2", "3"], result)

    def test_filter(self):
        result = self.properties.filter("foo.bar.")
        eq_("foo.bar.", result.prefix)
        eq_("", self.properties.prefix)
        eq_(1, result.get("baz"))
        eq_(2, result.get("qwe"))
    
    def test_filter_apply_double(self):
        result = self.properties.filter("foo.")
        result = result.filter("bar.")
        eq_("foo.bar.", result.prefix)
    
    def test_get_keys(self):
        filtered = self.properties.filter("foo.bar.")
        keys = filtered.get_keys()
        ok_("baz" in keys)
        ok_("qwe" in keys)
        eq_(2, len(keys))
