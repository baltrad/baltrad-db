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

from baltrad.bdbcommon import expr
from baltrad.bdbcommon.filter import Filter
from baltrad.bdbserver.sqla import filter, schema

from .conftest import bdb_backend

@pytest.mark.dbtest
class TestSqlAlchemyFilterManager(object):
    
    @pytest.fixture(autouse=True)
    def setup(self, bdb_backend):
        if not bdb_backend:
            pytest.skip("no backend defined")

        self.filter_manager = filter.SqlAlchemyFilterManager(bdb_backend)
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
        
        yield
        
        with bdb_backend.get_connection() as conn:
            with conn.begin():
                conn.execute(schema.filters.delete())

    @pytest.mark.dbtest
    def test_get_filter(self):
        result = self.filter_manager.get_filter("filter1")
        assert(self.filter1 == result)
    
    @pytest.mark.dbtest
    def test_get_filter_invalid(self):
        assert(None == self.filter_manager.get_filter("invalid"))
    
    @pytest.mark.dbtest
    def test_get_filter_names(self):
        self.filter_manager.add_filter(self.filter2)
        result = self.filter_manager.get_filter_names()
        assert(["filter1", "filter2"] == sorted(result))
    
    @pytest.mark.dbtest
    def test_remove_filter(self):
        self.filter_manager.add_filter(self.filter2)
        result = self.filter_manager.remove_filter("filter1")
        assert(True == result)
        filters = self.filter_manager.get_filter_names()
        assert(["filter2"] == filters)
    
    @pytest.mark.dbtest
    def test_remove_filter_nx(self):
        assert(False == self.filter_manager.remove_filter("filter2"))
    
    @pytest.mark.dbtest
    def test_update_filter(self):
        self.filter_manager.add_filter(self.filter2)
        self.filter1.expression = expr.eq(
            expr.attribute("where/elangle", "double"),
            expr.literal(2)
        )
        self.filter_manager.update_filter(self.filter1)
        result = self.filter_manager.get_filter("filter1")
        assert(self.filter1 == result)
        result = self.filter_manager.get_filter("filter2")
        assert(self.filter2 == result)
    
    @pytest.mark.dbtest
    def test_update_filter_nx(self):
        with pytest.raises(LookupError):
            self.filter_manager.update_filter(self.filter2)
