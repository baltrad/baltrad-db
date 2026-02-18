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
import optparse as pyoptparse
import os

import pytest

from baltrad.bdbcommon import optparse

def test_check_iso8601_datetime():
    result = optparse.check_iso8601_datetime(
        "option", None, "20120208T120102Z"
    )
    assert(result == datetime.datetime(2012, 2, 8, 12, 1, 2))

def test_check_iso8601_datetime_invalid():
    with pytest.raises(pyoptparse.OptionValueError):
        optparse.check_iso8601_datetime("option", None, "foo")

def test_check_list():
    result = optparse.check_list("option", None, "foo,bar,baz")
    assert(result == ["foo", "bar", "baz"])

def test_check_path():
    result = optparse.check_path("option", None, "./path/to/dir")
    expected = os.path.join(os.getcwd(), "path/to/dir")
    assert(result == expected)
