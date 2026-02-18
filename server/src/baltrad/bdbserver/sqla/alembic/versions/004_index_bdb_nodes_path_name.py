from alembic import op

# revision identifiers, used by Alembic.
revision = '004'
down_revision = '003'

def upgrade():
    print("Creating index on bdb_nodes(path, name)")
    op.create_index('bdb_nodes_path_name_key', 'bdb_nodes', ['path', 'name'])


def downgrade():
    op.drop_index('bdb_nodes_path_name_key', table_name='bdb_nodes')
