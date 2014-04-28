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
)

from sqlalchemy.types import (
    Integer,
    BigInteger,
    Text,
)

def upgrade(engine):
    meta = MetaData(bind=engine)
    nodes = Table("bdb_nodes", meta,
        # incomplete definition, only the parts necessary for this upgrade
        Column("id", Integer, primary_key=True),
        Column("file_id", Integer),
        Column("parent_id", Integer),
        Column("path", Text),
        Column("name", Text),
        Column("type", Integer),
    )
    nodes.c.id.alter(type=BigInteger)
    nodes.c.parent_id.alter(type=BigInteger)

    attrs = Table("bdb_attribute_values", meta,
        # with only the columns we are interested in
        Column("node_id", Integer)
    )
    attrs.c.node_id.alter(type=BigInteger)

def downgrade(engine):
    meta = MetaData(bind=engine)
    nodes = Table("bdb_nodes", meta,
        # incomplete definition, only the parts necessary for this upgrade
        Column("id", BigInteger, primary_key=True),
        Column("file_id", Integer),
        Column("parent_id", BigInteger),
        Column("path", Text),
        Column("name", Text),
        Column("type", Integer),
    )
    nodes.c.id.alter(type=Integer)
    nodes.c.parent_id.alter(type=Integer)

    attrs = Table("bdb_attribute_values", meta,
        # with only the columns we are interested in
        Column("node_id", BigInteger)
    )
    attrs.c.node_id.alter(type=Integer)