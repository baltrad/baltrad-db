from alembic import op
import sqlalchemy as sa

revision = '002'
down_revision = '001'

def upgrade():
    op.alter_column('bdb_attribute_values', 'value_int', new_column_name='value_long')
    op.alter_column('bdb_attribute_values', 'value_str', new_column_name='value_string')
    op.alter_column('bdb_attribute_values', 'value_bool', new_column_name='value_boolean')

def downgrade():
    op.alter_column('bdb_attribute_values', 'value_long', new_column_name='value_int')
    op.alter_column('bdb_attribute_values', 'value_string', new_column_name='value_str')
    op.alter_column('bdb_attribute_values', 'value_boolean', new_column_name='value_bool')