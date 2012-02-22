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

import json

from sqlalchemy import sql

from baltrad.bdbcommon import expr, filter
from baltrad.bdbserver import backend
from baltrad.bdbserver.sqla import schema

class SqlAlchemyFilterManager(backend.FilterManager):
    _select_filter_qry = sql.select(
        [schema.filters.c.expression],
        schema.filters.c.name==sql.bindparam("name")
    )

    _select_filter_names_qry = sql.select(
        [schema.filters.c.name],
    )

    def __init__(self, backend):
        self._backend = backend
    
    def get_filter_names(self):
        with self.get_connection() as conn:
            result = conn.execute(self._select_filter_names_qry)
            return [row[schema.filters.c.name] for row in result]

    def get_filter(self, name):
        with self.get_connection() as conn:
            stored_expr = conn.execute(
                self._select_filter_qry, name=name
            ).scalar()
            if not stored_expr:
                return None
            return filter.Filter(
                name,
                self._from_storable_expr(stored_expr)
            )
    
    def add_filter(self, filter):
        with self.get_connection() as conn:
            conn.execute(
                schema.filters.insert(),
                name=filter.name,
                expression=self._to_storable_expr(filter.expression)
            )
    
    def remove_filter(self, name):
        with self.get_connection() as conn:
            result = conn.execute(
                schema.filters.delete(
                    whereclause=schema.filters.c.name==name
                )
            )
            return bool(result.rowcount)
    
    def update_filter(self, filter):
        with self.get_connection() as conn:
            result = conn.execute(
                sql.update(
                    schema.filters,
                    whereclause=schema.filters.c.name==filter.name
                ),
                expression=self._to_storable_expr(filter.expression)
            )
            if not result.rowcount:
                raise LookupError("no filter found by name: %s" % filter.name)
    
    def _to_storable_expr(self, xpr):
        return json.dumps(expr.wrap_json(xpr))
    
    def _from_storable_expr(self, xpr):
        return expr.unwrap_json(json.loads(xpr))
    
    def get_connection(self):
        return self._backend.get_connection()
