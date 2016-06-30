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
## Database upgrade script that changes lo_id column in bdb_files to be of type oid 
## instead of integer
## @file
## @author Mats Vernersson, SMHI
## @date 2016-06-30

def upgrade(engine):
  dialect = engine.url.get_dialect().name
  if dialect == "postgresql":
    engine.execute("drop rule remove_lo on bdb_files")
    engine.execute("alter table bdb_files ALTER COLUMN lo_id TYPE oid")
    engine.execute("create rule remove_lo as on delete to bdb_files do select " + 
                           "lo_unlink(old.lo_id::oid) as lo_unlink")

def downgrade(engine):
  dialect = engine.url.get_dialect().name
  if dialect == "postgresql":
    engine.execute("drop rule remove_lo on bdb_files")
    engine.execute("alter table bdb_files ALTER COLUMN lo_id TYPE integer")
    engine.execute("create rule remove_lo as on delete to bdb_files do select " + 
                           "lo_unlink(old.lo_id::oid) as lo_unlink")
