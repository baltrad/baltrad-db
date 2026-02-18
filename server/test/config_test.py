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

import pytest

from baltrad.bdbserver import config

class TestProperties:
    @pytest.fixture(autouse=True)
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
        assert(1 == self.properties.get("foo.bar.baz"))
    
    def test_get_missing(self):
        with pytest.raises(config.PropertyLookupError):
            self.properties.get("qwe.asd")
    
    def test_get_missing_default(self):
        assert(2 == self.properties.get("qwe.asd", 2))
    
    def test_getitem(self):
        assert(2 == self.properties["foo.bar.qwe"])
    
    def test_getitem_missing(self):
        with pytest.raises(config.PropertyLookupError):
            self.properties["qwe.asd"]
    
    def test_get_boolean(self):
        assert(True == self.properties.get_boolean("qwe"))
        assert(False == self.properties.get_boolean("asd"))
    
    def test_get_boolean_invalid_value(self):
        with pytest.raises(config.PropertyValueError):
            self.properties.get_boolean("bar.foo.baz")
    
    def test_get_boolean_default_boolean(self):
        assert(True == self.properties.get_boolean("nxboolean", True))
        assert(False == self.properties.get_boolean("nxboolean", False))
    
    def test_get_boolean_default_none(self):
        assert(None == self.properties.get_boolean("nxboolean", None))
        
    def test_get_boolean_default_str(self):
        assert(True == self.properties.get_boolean("nxboolean", "yes"))
        assert(False == self.properties.get_boolean("nxboolean", "false"))
    
    def test_get_int(self):
        assert(1 == self.properties.get_int("foo.bar.baz"))
        assert(2 == self.properties.get_int("foo.bar.qwe"))
    
    def test_get_int_invalid_value(self):
        with pytest.raises(config.PropertyValueError):
            self.properties.get_int("bar.foo.baz")
    
    def test_get_int_default_int(self):
        assert(10 == self.properties.get_int("nxint", 10))

    def test_get_int_default_str(self):
        assert(10 == self.properties.get_int("nxint", "10"))
    
    def test_get_int_default_none(self):
        assert(None == self.properties.get_int("nxint", None))

    def test_get_list(self):
        assert(["a", "b", "c"] == self.properties.get_list("list", sep=","))
    
    def test_get_list_default_list(self):
        default = [1, 2, 3]
        assert(default == self.properties.get_list("nxlist", default=default))
    
    def test_get_list_default_str(self):
        result = self.properties.get_list("nxlist", default="1 2 3")
        assert(["1", "2", "3"] == result)

    def test_get_list_default_none(self):
        assert(None == self.properties.get_list("nxlist", None))

    def test_filter(self):
        result = self.properties.filter("foo.bar.")
        assert("foo.bar." == result.prefix)
        assert("" == self.properties.prefix)
        assert(1 == result.get("baz"))
        assert(2 == result.get("qwe"))
    
    def test_filter_apply_double(self):
        result = self.properties.filter("foo.")
        result = result.filter("bar.")
        assert("foo.bar." == result.prefix)
    
    def test_get_full_key(self):
        assert("foo" == self.properties.get_full_key("foo"))
        self.properties = self.properties.filter("foo.")
        assert("foo.bar" == self.properties.get_full_key("bar"))
    
    def test_get_keys(self):
        filtered = self.properties.filter("foo.bar.")
        keys = filtered.get_keys()
        assert("baz" in keys)
        assert("qwe" in keys)
        assert(2 == len(keys))
