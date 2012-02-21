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

from nose.tools import ok_, eq_

from baltrad.bdbserver.sqla import query

class TestExprToSql(object):
    def setup(self):
        self.evaluator = query.ExprToSql()
    
    def test_get_what_source_attr_column(self):
        col1 = self.evaluator.get_what_source_attr_column("what_source:name")
        col2 = self.evaluator.get_what_source_attr_column("what_source:name")
        # always accessed by the same column
        ok_(col1 is col2)
    
    def test_get_bdb_source_attr_column(self):
        col1 = self.evaluator.get_bdb_source_attr_column("_bdb/source:NOD")
        col2 = self.evaluator.get_bdb_source_attr_column("_bdb/source:NOD")
        # always accessed by the same column
        ok_(col1 is col2)

