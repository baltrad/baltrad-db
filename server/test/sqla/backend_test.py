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

from nose.tools import eq_, ok_, raises

from baltrad.bdbcommon import oh5
from baltrad.bdbserver.sqla import backend

def test_attribute_sql_values_str():
    attr = oh5.Attribute("name", "strval")
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_str": "strval"
    }
    eq_(expected, result)

def test_attribute_sql_values_int():
    attr = oh5.Attribute("name", 123)
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_int": 123
    }
    eq_(expected, result)

def test_attribute_sql_values_long():
    attr = oh5.Attribute("name", 123L)
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_int": 123L
    }
    eq_(expected, result)

def test_attribute_sql_values_float():
    attr = oh5.Attribute("name", 1.23)
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_double": 1.23
    }
    eq_(expected, result)

def test_attribute_sql_values_date():
    attr = oh5.Attribute("name", "20111213")
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_str": "20111213",
        "value_date": datetime.date(2011, 12, 13)
    }
    eq_(expected, result)

def test_attribute_sql_values_invalid_date():
    attr = oh5.Attribute("name", "20111313")
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_str": "20111313",
    }
    eq_(expected, result)

def test_attribute_sql_values_time():
    attr = oh5.Attribute("name", "131415")
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_str": "131415",
        "value_time": datetime.time(13, 14, 15)
    }
    eq_(expected, result)

def test_attribute_sql_values_invalid_time():
    attr = oh5.Attribute("name", "241415")
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_str": "241415",
    }
    eq_(expected, result)
