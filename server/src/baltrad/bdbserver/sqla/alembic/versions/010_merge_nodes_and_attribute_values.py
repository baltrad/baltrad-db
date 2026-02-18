"""
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

"""
## Merge bdb_nodes and bdb_attribute_values so that only one table is used. The performance penalty for using two
## tables seems to be in the range of double since each insert has to be done individually so that the
## attribute_value gets the correct node id. Could probably be solved by doing some fancy db-magic but that's
## better handled in the future if array searching is needed. Now, every value is just stored / node-id anyway.
## @file
## @author Anders Henja, SMHI
## @date 2019-01-22
from alembic import op
import sqlalchemy as sa
from sqlalchemy import text

revision = '010'
down_revision = '009'


def upgrade():
    op.add_column('bdb_nodes', sa.Column('value_long', sa.BigInteger(), nullable=True))
    op.add_column('bdb_nodes', sa.Column('value_string', sa.Text(), nullable=True))
    op.add_column('bdb_nodes', sa.Column('value_double', sa.Float(), nullable=True))
    op.add_column('bdb_nodes', sa.Column('value_boolean', sa.Boolean(), nullable=True))
    op.add_column('bdb_nodes', sa.Column('value_date', sa.Date(), nullable=True))
    op.add_column('bdb_nodes', sa.Column('value_time', sa.Time(), nullable=True))
    
    connection = op.get_bind()
    connection.execute(text(
        "UPDATE bdb_nodes bn SET "
        "value_long = bav.value_long, "
        "value_string = bav.value_string, "
        "value_double = bav.value_double, "
        "value_boolean = bav.value_boolean, "
        "value_date = bav.value_date, "
        "value_time = bav.value_time "
        "FROM bdb_attribute_values bav "
        "WHERE bn.id = bav.node_id"
    ))
    
    op.drop_table('bdb_attribute_values')


def downgrade():
    op.create_table(
        'bdb_attribute_values',
        sa.Column('node_id', sa.BigInteger(), nullable=False),
        sa.Column('value_long', sa.BigInteger(), nullable=True),
        sa.Column('value_string', sa.Text(), nullable=True),
        sa.Column('value_double', sa.Float(), nullable=True),
        sa.Column('value_boolean', sa.Boolean(), nullable=True),
        sa.Column('value_date', sa.Date(), nullable=True),
        sa.Column('value_time', sa.Time(), nullable=True),
        sa.ForeignKeyConstraint(['node_id'], ['bdb_nodes.id'], ondelete='CASCADE'),
        sa.PrimaryKeyConstraint('node_id')
    )
    
    connection = op.get_bind()
    connection.execute(text(
        "INSERT INTO bdb_attribute_values "
        "(node_id, value_long, value_string, value_double, value_boolean, value_date, value_time) "
        "SELECT id, value_long, value_string, value_double, value_boolean, value_date, value_time "
        "FROM bdb_nodes"
    ))
    
    # Drop value columns from bdb_nodes
    op.drop_column('bdb_nodes', 'value_time')
    op.drop_column('bdb_nodes', 'value_date')
    op.drop_column('bdb_nodes', 'value_boolean')
    op.drop_column('bdb_nodes', 'value_double')
    op.drop_column('bdb_nodes', 'value_string')
    op.drop_column('bdb_nodes', 'value_long')
