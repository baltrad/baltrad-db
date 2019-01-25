'''
Copyright (C) 2019- Swedish Meteorological and Hydrological Institute (SMHI)

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
## Merge bdb_nodes and bdb_attribute_values so that only one table is used. The performance penalty for using two
## tables seems to be in the range of double since each insert has to be done individually so that the
## attribute_value gets the correct node id. Could probably be solved by doing some fancy db-magic but that's
## better handled in the future if array searching is needed. Now, every value is just stored / node-id anyway.
## @file
## @author Anders Henja, SMHI
## @date 2019-01-22

import contextlib
import os

import progressbar

from sqlalchemy import (
  Column,
  ForeignKey,
  MetaData,
  PrimaryKeyConstraint,
  Table,
  sql,
  UniqueConstraint,  
)

from sqlalchemy.types import (
    BigInteger,
    Boolean,
    Date,
    Float,
    Integer,
    LargeBinary,
    Text,
    Time,
)

from sqlalchemy import (
  Column,
  MetaData,
  Table,
)

from sqlalchemy.types import (
  Integer,
  Text,
)

def upgrade(engine):
    meta = MetaData(bind=engine)
    nodes = Table("bdb_nodes", meta,
      Column("id", Integer, primary_key=True),
      Column("parent_id", Integer, ForeignKey("bdb_nodes.id")),
      Column("name", Text, nullable=False),
      Column("type", Integer, nullable=False),
      Column("value_long", BigInteger),
      Column("value_string", Text),
      Column("value_double", Float),
      Column("value_boolean", Boolean),
      Column("value_date", Date),
      Column("value_time", Time),
    )

    attribute_values = Table("bdb_attribute_values", meta,
      Column("node_id", Integer,
        ForeignKey(nodes.c.id, ondelete="CASCADE"),
        primary_key=True),
      Column("value_long", BigInteger, key="value_long"),
      Column("value_string", Text, key="value_string"),
      Column("value_double", Float),
      Column("value_boolean", Boolean, key="value_boolean"),
      Column("value_date", Date),
      Column("value_time", Time),
    )

    nodes.c.value_long.create()
    nodes.c.value_string.create()
    nodes.c.value_double.create()
    nodes.c.value_boolean.create()
    nodes.c.value_date.create()
    nodes.c.value_time.create()
   
    with contextlib.closing(engine.connect()) as conn:
        conn.execute("update bdb_nodes bn set value_long = bav.value_long, value_string=bav.value_string, value_double=bav.value_double, value_boolean=bav.value_boolean, value_date=bav.value_date, value_time=bav.value_time from bdb_attribute_values bav where bn.id = bav.node_id");
    
    attribute_values.drop();

def downgrade(engine):
    meta = MetaData(bind=engine)
    nodes = Table("bdb_nodes", meta,
      Column("id", Integer, primary_key=True),
      Column("parent_id", Integer, ForeignKey("bdb_nodes.id")),
      Column("name", Text, nullable=False),
      Column("type", Integer, nullable=False),
      Column("value_long", BigInteger),
      Column("value_string", Text),
      Column("value_double", Float),
      Column("value_boolean", Boolean),
      Column("value_date", Date),
      Column("value_time", Time),
    )
    
    attribute_values = Table("bdb_attribute_values", meta,
      Column("node_id", Integer,
        ForeignKey(nodes.c.id, ondelete="CASCADE"),
        primary_key=True),
      Column("value_long", BigInteger, key="value_long"),
      Column("value_string", Text, key="value_string"),
      Column("value_double", Float),
      Column("value_boolean", Boolean, key="value_boolean"),
      Column("value_date", Date),
      Column("value_time", Time),
    )
    
    attribute_values.create();
    
    with contextlib.closing(engine.connect()) as conn:
        conn.execute("insert into bdb_attribute_values (node_id,value_long,value_string,value_double,value_boolean,value_date,value_time) (select id,value_long,value_string,value_double,value_boolean,value_date,value_time from bdb_nodes)")
    
    nodes.c.value_long.drop()
    nodes.c.value_string.drop()
    nodes.c.value_double.drop()
    nodes.c.value_boolean.drop()
    nodes.c.value_date.drop()
    nodes.c.value_time.drop()
