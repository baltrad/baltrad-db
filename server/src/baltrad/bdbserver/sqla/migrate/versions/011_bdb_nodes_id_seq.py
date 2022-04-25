'''
Copyright (C) 2014- Swedish Meteorological and Hydrological Institute (SMHI)

This file is part of baltrad-db.

baltrad-db is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

baltrad-db is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.

'''
## Database upgrade script that changes some columns to use BigInteger instead of Integer since we hit the
## 2^31 limit.
## @file
## @author Anders Henja, SMHI
## @date 2014-04-28

import contextlib
import os

import progressbar

from sqlalchemy import (
    Column,
    MetaData,
    Table,
    sql,
    Sequence
)

from sqlalchemy.types import (
    Integer,
    BigInteger,
    Text,
)

def upgrade(engine):
    with contextlib.closing(engine.connect()) as conn:
        conn.execute("ALTER SEQUENCE bdb_nodes_id_seq as BIGINT"); # MAXVALUE 9223372036854775807

def downgrade(engine):
    with contextlib.closing(engine.connect()) as conn:
        r = conn.execute("select data_type from information_schema.columns where table_name='bdb_nodes' and column_name='id'").fetchall()
        if len(r) > 0 and len(r[0]) > 0:
            dt = r[0][0]
            if td.lower()  == "integer":
                conn.execute("ALTER SEQUENCE bdb_nodes_id_seq as INTEGER");
      
