from alembic import op
import sqlalchemy as sa
from sqlalchemy import select, func, and_

revision = '005'
down_revision = '004'

meta = sa.MetaData()

files = sa.Table("bdb_files", meta,
    sa.Column("id", sa.Integer, primary_key=True),
)

nodes = sa.Table("bdb_nodes", meta,
    sa.Column("id", sa.Integer, primary_key=True),
    sa.Column("file_id", sa.Integer, sa.ForeignKey("bdb_files.id")),
    sa.Column("path", sa.Text),
    sa.Column("name", sa.Text),
)

attribute_values = sa.Table("bdb_attribute_values", meta,
    sa.Column("node_id", sa.Integer, sa.ForeignKey("bdb_nodes.id")),
    sa.Column("value_string", sa.Text),
)

def upgrade():
    op.create_table(
        'bdb_what_source_kvs',
        sa.Column('id', sa.Integer(), primary_key=True),
        sa.Column('key', sa.Text(), nullable=False),
        sa.Column('value', sa.Text(), nullable=False),
        sa.UniqueConstraint('key', 'value')
    )
    
    op.create_table(
        'bdb_file_what_source',
        sa.Column('file_id', sa.Integer(), sa.ForeignKey('bdb_files.id', ondelete="CASCADE"), nullable=False),
        sa.Column('source_kv_id', sa.Integer(), sa.ForeignKey('bdb_what_source_kvs.id'), nullable=False),
        sa.PrimaryKeyConstraint('file_id', 'source_kv_id')
    )
    bind = op.get_bind()
    _upgrade_what_source(bind)

def downgrade():
    op.drop_table('bdb_file_what_source')
    op.drop_table('bdb_what_source_kvs')

def get_base_stmt():
    return (
        select(files.c.id, attribute_values.c.value_string)
        .select_from(files.join(nodes).join(attribute_values))
        .where(
            and_(
                nodes.c.path == "/what",
                nodes.c.name == "source"
            )
        )
    )

def _upgrade_what_source(conn):
    base_stmt = get_base_stmt()
    
    count_stmt = select(func.count()).select_from(base_stmt.alias("sub"))
    rowcount = conn.execute(count_stmt).scalar()
    
    if not rowcount or rowcount == 0:
        return

    print(f"Selecting {rowcount} /what/source attributes for update")
    
    result = conn.execute(base_stmt)

    print("associating fine-grained /what/source values with files")
    pbar = progressbar.ProgressBar(
        widgets=[progressbar.Percentage()],
        maxval=rowcount
    ).start()

    for row in result:
        source = oh5.Source.from_string(row.value_string)
        backend.associate_what_source(conn, row.id, source)
        pbar.update(pbar.currval + 1)
        
    pbar.finish()