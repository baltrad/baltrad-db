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

def get_server_version(conn):
    try:
        v = conn.execute("show server_version").fetchall()
        if len(v) > 0 and len(v[0]) > 0:
            s = v[0][0].split(" ")
            if len(s) > 0 and s[0].find(".") > 0:
                return int(s[0].split(".")[0])
    except:
        pass
        
    return 10 # Psql 9 has been deprecated


def upgrade(engine):
    with contextlib.closing(engine.connect()) as conn:
        v = get_server_version(conn)
        if v >= 10:
            conn.execute("ALTER SEQUENCE bdb_nodes_id_seq as BIGINT") # MAXVALUE 9223372036854775807
        conn.execute("ALTER SEQUENCE bdb_nodes_id_seq CYCLE")


def downgrade(engine):
    with contextlib.closing(engine.connect()) as conn:
        conn.execute("ALTER SEQUENCE bdb_nodes_id_seq NOCYCLE")
      
