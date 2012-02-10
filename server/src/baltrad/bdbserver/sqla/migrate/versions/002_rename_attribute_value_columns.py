import migrate

from sqlalchemy import (
    Column,
    MetaData,
    Table,
)
from sqlalchemy.types import (
    BigInteger,
    Boolean,
    Text,
)


def upgrade(migrate_engine):
    meta = MetaData(bind=migrate_engine)
    attribute_values = Table("bdb_attribute_values", meta,
        # with only the columns we are interested in
        Column("value_int", BigInteger),
        Column("value_str", Text),
        Column("value_bool", Boolean),
    )

    attribute_values.c.value_int.alter(name="value_long")
    attribute_values.c.value_str.alter(name="value_string")
    attribute_values.c.value_bool.alter(name="value_boolean")

def downgrade(migrate_engine):
    meta = MetaData(bind=migrate_engine)
    attribute_values = Table("bdb_attribute_values", meta,
        # with only the columns we are interested in
        Column("value_long", BigInteger),
        Column("value_string", Text),
        Column("value_boolean", Boolean),
    )

    attribute_values.c.value_long.alter(name="value_int")
    attribute_values.c.value_string.alter(name="value_str")
    attribute_values.c.value_boolean.alter(name="value_bool")
