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

from baltrad.bdbcommon import oh5

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

    nodes_query = sql.select(
        [nodes.c.id, nodes.c.parent_id, nodes.c.name, nodes.c.type],
        order_by=[
            sql.asc(nodes.c.file_id),
            sql.asc(nodes.c.parent_id).nullsfirst()
        ],
    )
    
    update_nodes_query = sql.update(
        nodes,
        whereclause=(nodes.c.parent_id==sql.bindparam("_parent_id")),
        values={"path" :sql.bindparam("_path")}
    )

    def __call__(self, engine):
        # note that schema changes are not done transactionally
        self.meta.bind = engine
        self.nodes.c.path.create()
        with contextlib.closing(engine.connect()) as conn:
            self.add_path(conn)
        self.nodes.c.path.alter(nullable=False)
    
    def add_path(self, conn):
        rowcount = conn.execute(
            sql.select([sql.func.count(self.nodes.c.id)])
        ).scalar()
        if rowcount == 0:
            return
        print("Selecting %d rows in bdb_nodes for update" % rowcount)
        result = conn.execute(self.nodes_query)

        print("Adding bdb_nodes.path")
        pbar = progressbar.ProgressBar(
            widgets=[progressbar.Percentage()],
            maxval=rowcount
        ).start()

        paths = {}
        for row in result:
            # no parent marks the root node of a new file
            if row[self.nodes.c.parent_id] is None:
                if paths:
                    self.update_paths(conn, paths)
                paths.clear()
                path = "/"
            else:
                path = os.path.join(
                    paths.get(row[self.nodes.c.parent_id]),
                    row[self.nodes.c.name]
                )
            if row[self.nodes.c.type] != oh5.Attribute.type_id:
                paths[row[self.nodes.c.id]] = path
            pbar.update(pbar.currval + 1)
        # set path for nodes in the last file
        self.update_paths(conn, paths)
        # set the path on root nodes
        conn.execute(sql.update(
            self.nodes,
            whereclause=(self.nodes.c.parent_id==None),
            values={"path": "/"}
        ))
        pbar.finish()

    def update_paths(self, conn, paths):
        conn.execute(
            self.update_nodes_query,
            [{"_parent_id": k, "_path": v} for k, v in paths.items()]
        )

def upgrade(engine):
    upgrader = Upgrader()
    upgrader(engine)

def downgrade(engine):
    meta = MetaData(bind=engine)
    nodes = Table("bdb_nodes", meta,
        # incomplete definition, only the parts necessary for this upgrade
        Column("path", Text),
    )
    nodes.c.path.drop()
