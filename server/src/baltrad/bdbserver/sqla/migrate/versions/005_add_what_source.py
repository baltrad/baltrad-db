import contextlib

import progressbar

from sqlalchemy import (
    Column,
    ForeignKey,
    MetaData,
    PrimaryKeyConstraint,
    Table,
    UniqueConstraint,
    sql,
)

meta = MetaData()

from sqlalchemy.types import (
    Integer,
    Text,
)

from baltrad.bdbcommon import oh5
from baltrad.bdbserver.sqla import backend

files = Table("bdb_files", meta,
    Column("id", Integer, primary_key=True),
    # columns omitted
)

nodes = Table("bdb_nodes", meta,
    Column("id", Integer, primary_key=True),
    Column("file_id", Integer,
        ForeignKey(files.c.id, ondelete="CASCADE"),
        nullable=False),
    Column("path", Text, nullable=False),
    Column("name", Text, nullable=False),
    # columns omitted
)

attribute_values = Table("bdb_attribute_values", meta,
    Column("node_id", Integer,
        ForeignKey(nodes.c.id, ondelete="CASCADE"),
        primary_key=True),
    Column("value_string", Text),
    # columns omitted
)

what_source_kvs = Table("bdb_what_source_kvs", meta,
    Column("id", Integer, primary_key=True),
    Column("key", Text, nullable=False),
    Column("value", Text, nullable=False),
    UniqueConstraint("key", "value"),
)

file_what_source = Table("bdb_file_what_source", meta,
    Column("file_id", Integer,
        ForeignKey(files.c.id, ondelete="CASCADE"),
        nullable=False),
    Column("source_kv_id", Integer, ForeignKey(what_source_kvs.c.id),
        nullable=False),
    PrimaryKeyConstraint("file_id", "source_kv_id"),
)

_what_source_qry = sql.select(
    [files.c.id, attribute_values.c.value_string],
    whereclause=sql.and_(
        nodes.c.path=="/what",
        nodes.c.name=="source"
    ),
    from_obj=files.join(nodes).join(attribute_values),
)

def _upgrade_what_source(conn):
    rowcount = conn.execute(_what_source_qry.alias("cnt").count()).scalar()
    if rowcount == 0:
        return
    print("Selecting %d /what/source attributes for update" % rowcount)
    result = conn.execute(_what_source_qry)

    print("associating fine-grained /what/source values with files")
    pbar = progressbar.ProgressBar(
        widgets=[progressbar.Percentage()],
        maxval=rowcount
    ).start()

    for row in result:
        pbar.update(pbar.currval + 1)
        source = oh5.Source.from_string(row[attribute_values.c.value_string])
        backend.associate_what_source(conn, row[files.c.id], source)
    pbar.finish()

def upgrade(engine):
    meta.bind = engine

    what_source_kvs.create()
    file_what_source.create()
    with contextlib.closing(engine.connect()) as conn:
        with conn.begin():
            _upgrade_what_source(conn)


def downgrade(engine):
    meta.bind = engine

    file_what_source.drop()
    what_source_kvs.drop()
