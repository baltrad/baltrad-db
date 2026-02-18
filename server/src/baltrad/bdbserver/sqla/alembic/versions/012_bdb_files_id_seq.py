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
## @date 2022-05-18

from alembic import op
from sqlalchemy import text

# revision identifiers, used by Alembic.
revision = '012'
down_revision = '011'


def get_server_version(conn):
    try:
        v = conn.execute(text("show server_version")).fetchall()
        if len(v) > 0 and len(v[0]) > 0:
            s = v[0][0].split(" ")
            if len(s) > 0 and s[0].find(".") > 0:
                return int(s[0].split(".")[0])
    except:
        pass
    return 10  # Psql 9 has been deprecated


def upgrade():
    connection = op.get_bind()
    v = get_server_version(connection)
    if v >= 10:
        connection.execute(text("ALTER SEQUENCE bdb_files_id_seq as BIGINT"))  # MAXVALUE 9223372036854775807
    connection.execute(text("ALTER SEQUENCE bdb_files_id_seq CYCLE"))


def downgrade():
    connection = op.get_bind()
    connection.execute(text("ALTER SEQUENCE bdb_files_id_seq NO CYCLE"))
