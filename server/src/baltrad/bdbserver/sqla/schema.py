from sqlalchemy import (
    Column,
    ForeignKey,
    Index,
    MetaData,
    PrimaryKeyConstraint,
    Table,
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

meta = MetaData()

sources = Table("bdb_sources", meta,
    Column("id", Integer, primary_key=True),
    Column("name", Text, unique=True, nullable=False),
)

source_kvs = Table("bdb_source_kvs", meta,
    Column("source_id", Integer,
        ForeignKey(sources.c.id, ondelete="CASCADE"),
        nullable=False),
    Column("key", Text, nullable=False),
    Column("value", Text, nullable=False),
    PrimaryKeyConstraint("source_id", "key"),
)

files = Table("bdb_files", meta,
    Column("id", Integer, primary_key=True),
    Column("uuid", Text, unique=True, nullable=False),
    Column("hash", Text, nullable=False),
    Column("stored_date", Date, nullable=False),
    Column("stored_time", Time, nullable=False),
    Column("what_object", Text, nullable=False),
    Column("what_date", Date, nullable=False),
    Column("what_time", Time, nullable=False),
    Column("source_id", Integer, ForeignKey(sources.c.id), nullable=False),
    Column("size", Integer, nullable=False),
    Column("lo_id", Integer, key="oid"),
    UniqueConstraint("hash", "source_id"),
)

file_data = Table("bdb_file_data", meta,
    Column("id", Integer, primary_key=True),
    Column("data", LargeBinary, nullable=False)
)

nodes = Table("bdb_nodes", meta,
    Column("id", Integer, primary_key=True),
    Column("file_id", Integer,
        ForeignKey(files.c.id, ondelete="CASCADE"),
        nullable=False),
    Column("parent_id", Integer, ForeignKey("bdb_nodes.id")),
    Column("name", Text, nullable=False),
    Column("type", Integer, nullable=False),
    UniqueConstraint("parent_id", "name"),
)

attribute_values = Table("bdb_attribute_values", meta,
    Column("node_id", Integer,
        ForeignKey(nodes.c.id, ondelete="CASCADE"),
        primary_key=True),
    Column("value_long", BigInteger),
    Column("value_string", Text),
    Column("value_double", Float),
    Column("value_boolean", Boolean),
    Column("value_date", Date),
    Column("value_time", Time),
)
