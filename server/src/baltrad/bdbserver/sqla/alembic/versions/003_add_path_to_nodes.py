import contextlib
import os

import progressbar

from sqlalchemy import (
    Column,
    MetaData,
    Table,
    sql,
)

from sqlalchemy.types import (
    Integer,
    Text,
)

from sqlalchemy import select, func, update, bindparam

from baltrad.bdbcommon import oh5
from alembic import op

revision = '003'
down_revision = '002'

class Upgrader(object):
    meta = MetaData()

    nodes = Table("bdb_nodes", meta,
        # incomplete definition, only the parts necessary for this upgrade
        Column("id", Integer, primary_key=True),
        Column("file_id", Integer),
        Column("parent_id", Integer),
        Column("path", Text),
        Column("name", Text),
        Column("type", Integer),
    )

    nodes_query = select(nodes.c.id, nodes.c.parent_id, nodes.c.name, nodes.c.type).order_by(sql.asc(nodes.c.file_id),sql.asc(nodes.c.parent_id).nullsfirst())

    update_nodes_query = update(nodes).where(nodes.c.parent_id == bindparam("_parent_id")).values(path=bindparam("_path"))

    def __call__(self):
        op.add_column('bdb_nodes', Column('path', Text()))
        self.add_path()
        op.alter_column('bdb_nodes', 'path', nullable=False)
    
    def add_path(self):
        conn = op.get_bind()

        rowcount = conn.execute(select(func.count(self.nodes.c.id))).scalar()
        
        if rowcount == 0 or rowcount is None:
            return

        print(f"Selecting {rowcount} rows in bdb_nodes for update")
        
        result = conn.execute(self.nodes_query)

        pbar = progressbar.ProgressBar(
            widgets=[progressbar.Percentage()],
            maxval=rowcount
        ).start()

        paths = {}
        for row in result:
            parent_id = row.parent_id
            node_id = row.id
            node_name = row.name
            node_type = row.type

            # no parent marks the root node of a new file
            if parent_id is None:
                if paths:
                    self.update_paths(conn, paths)
                paths.clear()
                path = "/"
            else:
                parent_path = paths.get(parent_id)
                path = os.path.join(parent_path, node_name) if parent_path is not None else "/"

            if node_type != oh5.Attribute.type_id:
                paths[node_id] = path
                
            pbar.update(pbar.currval + 1)

        if paths:
            self.update_paths(conn, paths)

        conn.execute(
            update(self.nodes)
            .where(self.nodes.c.parent_id == None)
            .values(path="/")
        )
        pbar.finish()

    def update_paths(self, conn, paths):
        conn.execute(
            self.update_nodes_query,
            [{"_parent_id": k, "_path": v} for k, v in paths.items()]
        )

def upgrade():
    upgrader = Upgrader()
    upgrader()

def downgrade():
    op.drop_column('bdb_nodes', 'path')
