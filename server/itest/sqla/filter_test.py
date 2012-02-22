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
from nose.plugins.attrib import attr
from nose.plugins.skip import SkipTest

from baltrad.bdbcommon import expr
from baltrad.bdbcommon.filter import Filter
from baltrad.bdbserver.sqla import filter, schema

from . import get_backend

class TestSqlAlchemyFilterManager(object):
    backend = None
    
    @classmethod
    def setup_class(cls):
        cls.backend = get_backend()
    
    def setup(self):
        if not self.backend:
            raise SkipTest("no backend defined")

        self.filter_manager = filter.SqlAlchemyFilterManager(self.backend)
        self.filter1 = Filter(
            "filter1",
            expr.eq(
                expr.attribute("what/object", "string"),
                expr.literal("PVOL")
            )
        )
        self.filter2 = Filter(
            "filter2",
            expr.eq(
                expr.attribute("where/elangle", "double"),
                expr.literal(1)
            )
        )
        self.filter_manager.add_filter(self.filter1)
    
    def teardown(self):
        with self.backend.get_connection() as conn:
            conn.execute(schema.filters.delete())
    
    @attr("dbtest")
    def test_get_filter(self):
        result = self.filter_manager.get_filter("filter1")
        eq_(self.filter1, result)
    
    def test_get_filter_invalid(self):
        eq_(None, self.filter_manager.get_filter("invalid"))
    
    @attr("dbtest")
    def test_get_filter_names(self):
        result = self.filter_manager.get_filter_names()
        self.filter_manager.add_filter(self.filter2)
        ok_(["filter1", "filter2"], sorted(result))
    
    @attr("dbtest")
    def test_remove_filter(self):
        self.filter_manager.add_filter(self.filter2)
        result = self.filter_manager.remove_filter("filter1")
        eq_(True, result)
        filters = self.filter_manager.get_filter_names()
        eq_(["filter2"], filters)
    
    @attr("dbtest")
    def test_remove_filter_nx(self):
        eq_(False, self.filter_manager.remove_filter("filter2"))
    
    @attr("dbtest")
    def test_update_filter(self):
        self.filter_manager.add_filter(self.filter2)
        self.filter1.expression = expr.eq(
            expr.attribute("where/elangle", "double"),
            expr.literal(2)
        )
        self.filter_manager.update_filter(self.filter1)
        result = self.filter_manager.get_filter("filter1")
        eq_(self.filter1, result)
        result = self.filter_manager.get_filter("filter2")
        eq_(self.filter2, result)
    
    @attr("dbtest")
    @raises(LookupError)
    def test_update_filter_nx(self):
        self.filter_manager.update_filter(self.filter2)
