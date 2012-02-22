from sqlalchemy import (
    Column,
    MetaData,
    Table,
)

from sqlalchemy.types import (
    Integer,
    Text,
)

meta = MetaData()

filters = Table("bdb_filters", meta,
    Column("id", Integer, primary_key=True),
    Column("name", Text, nullable=False),
    Column("expression", Text, nullable=False),
)

def upgrade(engine):
    meta.bind = engine
    filters.create()


def downgrade(engine):
    meta.bind = engine
    filters.drop()
