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
import mock

from baltrad.bdbcommon import expr
from baltrad.bdbclient import db

class TestAttributeQuery(object):
    def setup(self):
        self.query = db.AttributeQuery()

    def test_append_filter(self):
        self.query.append_filter(mock.sentinel.expr1)
        eq_(mock.sentinel.expr1, self.query.filter)
        self.query.append_filter(mock.sentinel.expr2)
        expected = expr.and_(mock.sentinel.expr1, mock.sentinel.expr2)
        eq_(expected, self.query.filter)
    
    @mock.patch("json.dumps")
    def test_to_json(self, json_dumps):
        json_dumps.return_value = mock.sentinel.json

        self.query.fetch("uuid", expr.attribute("file:uuid", "string"))
        self.query.filter = expr.eq(
            expr.attribute("what/object", "string"), "PVOL"
        )
        self.query.order = [
            expr.desc(expr.attribute("file:uuid", "string")),
        ]
        self.query.group = [
            expr.attribute("file:uuid", "string"),
        ]
        self.query.limit = 10
        self.query.skip = 20

        json_repr = {
            "fetch": {
                "uuid": ["list", ["symbol", "attr"], "file:uuid", "string"]
            },
            "filter": ["list", ["symbol", "="], ["list", ["symbol", "attr"], "what/object", "string"], "PVOL"],
            "order": [["list", ["symbol", "desc"], ["list", ["symbol", "attr"], "file:uuid", "string"]]],
            "group": [["list", ["symbol", "attr"], "file:uuid", "string"]],
            "limit": 10,
            "skip": 20,
        }

        eq_(mock.sentinel.json, self.query.to_json())
        json_dumps.assert_called_once_with(json_repr)
