from sqlalchemy import (
    event,
    Column,
    DDL,
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

# remove large object when file is deleted in postgresql
event.listen(
    files,
    "after_create",
    DDL(
        "CREATE RULE remove_lo AS ON DELETE TO bdb_files "
            "DO SELECT lo_unlink(OLD.lo_id)"
    ).execute_if(dialect="postgresql")
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
    Column("value_int", BigInteger, key="value_long"),
    Column("value_str", Text, key="value_string"),
    Column("value_double", Float),
    Column("value_bool", Boolean, key="value_boolean"),
    Column("value_date", Date),
    Column("value_time", Time),
)


##
# INDEXES
##

Index("bdb_files_what_object_key", files.c.what_object)
Index("bdb_files_what_date_key", files.c.what_date)
Index("bdb_files_what_time_key", files.c.what_time)

event.listen(
    files,
    "after_create",
    DDL(
        "CREATE INDEX bdb_files_combined_datetime_key ON "
        "bdb_files((what_date + what_time))"
    ).execute_if(dialect="postgresql")
)

event.listen(
    files,
    "after_create",
    DDL(
        "CREATE INDEX bdb_files_stored_datetime_key ON "
        "bdb_files((stored_date + stored_time))"
    ).execute_if(dialect="postgresql")
)

Index("bdb_nodes_id_name_key", nodes.c.id, nodes.c.name)
Index("bdb_nodes_file_id_name_key", nodes.c.file_id, nodes.c.name)
