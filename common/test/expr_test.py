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

from baltrad.bdbcommon import expr

def test_symbol_ctor():
    result = expr.symbol("foo")
    assert(isinstance(result, expr.Symbol))
    assert(result.value == "foo")

def test_symbol_eq():
    sym1 = expr.symbol("foo")
    sym2 = expr.symbol("bar")
    sym3 = expr.symbol("foo")
    assert(sym1 == sym1)
    assert(sym1 == sym3)
    assert(sym1 != sym2)
    assert(sym2 != sym3)

def test_unwrap_json():
    json = ["list", 
        ["symbol", "in"],
        ["list", ["symbol", "attr"], "what/object", "string"],
        ["list", "PVOL", "SCAN"]
    ]
    expected = [
        expr.symbol("in"),
        [expr.symbol("attr"), "what/object", "string"],
        ["PVOL", "SCAN"]
    ]
    assert(expr.unwrap_json(json) == expected)

def test_wrap_json():
    xpr = [
        expr.symbol("in"),
        [expr.symbol("attr"), "what/object", "string"],
        ["PVOL", "SCAN"]
    ]
    expected = ["list", 
        ["symbol", "in"],
        ["list", ["symbol", "attr"], "what/object", "string"],
        ["list", "PVOL", "SCAN"]
    ]
    assert(expr.wrap_json(xpr) == expected)

def test_literal_date():
    result = expr.literal(datetime.date(2011, 12, 13))
    assert(result == [expr.symbol("date"), 2011, 12, 13])

def test_literal_time():
    result = expr.literal(datetime.time(13, 14, 15))
    assert(result == [expr.symbol("time"), 13, 14, 15])

def test_literal_datetime():
    result = expr.literal(datetime.datetime(2011, 12, 13, 14, 15, 16))
    assert(result == [expr.symbol("datetime"), 2011, 12, 13, 14, 15, 16])

def test_literal_timedelta():
    result = expr.literal(datetime.timedelta(123, 456))
    assert(result == [expr.symbol("interval"), 123, 456])

