from sqlalchemy import (
    Column,
    Index,
    MetaData,
    Table,
)

from sqlalchemy.types import (
    Text,
)

meta = MetaData()

nodes = Table("bdb_nodes", meta,
    Column("path", Text, nullable=False),
    Column("name", Text, nullable=False),
    # columns omitted
)
idx = Index("bdb_nodes_path_name_key", nodes.c.path, nodes.c.name)

def upgrade(engine):
    meta.bind = engine
    print "Creating index on bdb_nodes(path, name)"
    idx.create()

def downgrade(engine):
    meta.bind = engine
    idx.drop()
