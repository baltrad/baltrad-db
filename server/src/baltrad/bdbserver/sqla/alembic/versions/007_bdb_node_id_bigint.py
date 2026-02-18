"""
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

"""
from alembic import op
import sqlalchemy as sa

revision = '007'
down_revision = '006'

def upgrade():
    # Change bdb_nodes columns to BigInteger
    op.alter_column('bdb_nodes', 'id',
                    existing_type=sa.Integer(),
                    type_=sa.BigInteger(),
                    existing_nullable=False)
    
    op.alter_column('bdb_nodes', 'parent_id',
                    existing_type=sa.Integer(),
                    type_=sa.BigInteger(),
                    existing_nullable=True)
    
    # Change bdb_attribute_values.node_id to BigInteger
    op.alter_column('bdb_attribute_values', 'node_id',
                    existing_type=sa.Integer(),
                    type_=sa.BigInteger(),
                    existing_nullable=False)


def downgrade():
    # Revert bdb_nodes columns to Integer
    op.alter_column('bdb_nodes', 'id',
                    existing_type=sa.BigInteger(),
                    type_=sa.Integer(),
                    existing_nullable=False)
    
    op.alter_column('bdb_nodes', 'parent_id',
                    existing_type=sa.BigInteger(),
                    type_=sa.Integer(),
                    existing_nullable=True)
    
    # Revert bdb_attribute_values.node_id to Integer
    op.alter_column('bdb_attribute_values', 'node_id',
                    existing_type=sa.BigInteger(),
                    type_=sa.Integer(),
                    existing_nullable=False)
