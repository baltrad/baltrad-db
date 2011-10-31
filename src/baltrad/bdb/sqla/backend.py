# Copyright 2010-2011 Estonian Meteorological and Hydrological Institute
# 
# This file is part of baltrad-db.
# 
# baltrad-db is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# baltrad-db is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.

import datetime
import os
import stat
import uuid

from sqlalchemy import engine, sql

from . import schema
from .. import oh5
from .. backend import Backend, DuplicateEntry

class GenericDatabaseFileStorage(object):
    def store(self, conn, path):
        return conn.execute(
            schema.file_data.insert(),
            data=open(path, "r").read()
        ).inserted_primary_key[0]
    
    def read(self, conn, oid):
        return conn.execute(
            sql.select(
                [schema.file_data.c.data],
                schema.file_data.c.id==oid
            )
        ).scalar()
    
    def remove(self, conn, oid):
        conn.execute(
            schema.file_data.delete().where(schema.file_data.c.id==oid)
        )

class Psycopg2DatabaseFileStorage(object):
    def store(self, conn, path):
        lobj = conn.connection.lobject(new_file=path)
        return lobj.oid

    def read(self, conn, oid):
        lobj = conn.connection.lobject(oid=oid, mode="r")
        return lobj.read()
    
    def remove(self, conn, oid):
        lobj = conn.connection.lobject(oid=oid)
        lobj.unlink()

class SqlAlchemyBackend(Backend):
    """A backend using sqlalchemy to store files in a relational database
    """

    def __init__(self, engine_or_url):
        """Constructor

        :param engine_or_url: sqlalchemy engine or database url
        """
        if isinstance(engine_or_url, basestring):
            self._engine = engine.create_engine(engine_or_url)
        else:
            self._engine = engine_or_url
        self._hasher = oh5.MetadataHasher()

        if self._engine.driver == "psycopg2":
            self._file_storage = Psycopg2DatabaseFileStorage()
        else:
            self._file_storage = GenericDatabaseFileStorage()

    @staticmethod
    def create_from_config(config):
        return SqlAlchemyBackend(config["url"])

        
    def store_file(self, path):
        conn = self._engine.connect()

        meta = oh5.Metadata.from_file(path)

        metadata_hash = self._hasher.hash(meta)
        source_id = self.get_source_id(meta.source())
        if not source_id:
            raise LookupError("failed to look up source for " +
                              meta.source().to_string())
        if _has_file_by_hash_and_source(conn, metadata_hash, source_id):
            raise DuplicateEntry()

        meta.bdb_uuid = str(uuid.uuid4())
        meta.bdb_file_size = os.stat(path)[stat.ST_SIZE]
        meta.bdb_metadata_hash = metadata_hash
        stored_timestamp = datetime.datetime.utcnow()
        meta.bdb_stored_date = stored_timestamp.date()
        meta.bdb_stored_time = stored_timestamp.time()
        source = self.get_source_by_id(source_id)
        meta.bdb_source = source.to_string(True)

        trans = conn.begin()
        try:
            file_id = _insert_file(conn, meta, source_id)
            _insert_metadata(conn, meta, file_id)
            oid = self._file_storage.store(conn, path)
            conn.execute(
                schema.files.update().where(schema.files.c.id==file_id),
                lo_id=oid
            )
            trans.commit()
        except:
            trans.rollback()
            raise

        return meta

    def get_file(self, uuid):
        conn = self.get_connection()
        qry = sql.select(
            [schema.files.c.lo_id],
            schema.files.c.uuid==str(uuid)
        )
        oid = conn.execute(qry).scalar()
        return self._file_storage.read(conn, oid);
    
    def get_file_metadata(self, uuid):
        conn = self.get_connection()

        qry = sql.select(
            [schema.files],
            schema.files.c.uuid==str(uuid)
        )
        row = conn.execute(qry).fetchone()
        if not row:
            return None

        file_id = row[schema.files.c.id]

        meta = _select_metadata(conn, file_id)
        meta.bdb_uuid = row[schema.files.c.uuid]
        meta.bdb_file_size = row[schema.files.c.size]
        source_id = row[schema.files.c.source_id]
        meta.bdb_source = self.get_source_by_id(source_id).to_string(True)
        meta.bdb_stored_date = row[schema.files.c.stored_date]
        meta.bdb_stored_time = row[schema.files.c.stored_time]

        return meta
     
    def remove_file(self, uuid):
        conn = self.get_connection()
        qry = schema.files.delete(
            schema.files.c.uuid==str(uuid)
        )
        return bool(conn.execute(qry).rowcount)

    def file_source(self, uuid):
        qry = sql.select(
            [schema.source_kvs],
            from_obj=[schema.files.join(schema.sources).join(schema.source_kvs)],
        )
        qry = qry.where(schema.files.c.uuid==str(uuid))

        conn = self.get_connection()
        result = conn.execute(qry).fetchall()
        d = {}
        for row in result:
            key = row[schema.source_kvs.key]
            value = row[schema.source_kvs.value]
            d[key] = value
        return d
    
    def get_connection(self):
        return self._engine.connect()
    
    def get_source_id(self, source):
        where = sql.literal(False)
        for key, value in source.iteritems():
            where = sql.or_(
                where,
                sql.and_(
                    schema.source_kvs.c.key==key,
                    schema.source_kvs.c.value==value
                )
            )

        qry = sql.select(
            [schema.source_kvs.c.source_id],
            where,
            distinct=True
        )

        conn = self.get_connection()
        return conn.execute(qry).scalar()
            
    def get_source_by_id(self, source_id):
        source = oh5.Source()
        conn = self.get_connection()

        qry = sql.select(
            [schema.sources.c.name],
            schema.sources.c.id==source_id
        )
        source.name = conn.execute(qry).scalar()

        qry = sql.select(
            [schema.source_kvs],
            schema.source_kvs.c.source_id==source_id
        )
        
        for row in conn.execute(qry).fetchall():
            source[row["key"]] = row["value"]
        return source
    
    def get_sources(self):
        conn = self.get_connection()

        qry = sql.select(
            [schema.sources, schema.source_kvs],
            from_obj=schema.source_kvs.join(schema.sources)
        )

        sources = {}

        for row in conn.execute(qry):
            name = row["name"]
            source = sources.setdefault(name, oh5.Source({"_name": name}))
            source[row["key"]] = row["value"]
        return sources.values()
    
def _insert_file(conn, meta, source_id):
    return conn.execute(
        schema.files.insert(),
        uuid=meta.bdb_uuid,
        hash=meta.bdb_metadata_hash,
        source_id=source_id,
        stored_date=meta.bdb_stored_date,
        stored_time=meta.bdb_stored_time,
        what_object=meta.what_object,
        what_date=meta.what_date,
        what_time=meta.what_time,
        size=meta.bdb_file_size,
    ).inserted_primary_key[0]

def _insert_metadata(conn, meta, file_id):
    node_ids = {}

    for node in meta.iternodes():
        if node.path().startswith("/_bdb"):
            continue
        node_id = conn.execute(
            schema.nodes.insert(),
            file_id=file_id,
            parent_id=node_ids.get(node.parent, None),
            name=node.name,
            type=node.type_id,
        ).inserted_primary_key[0]
        node_ids[node] = node_id
        if isinstance(node, oh5.Attribute):
            _insert_attribute_value(conn, node, node_id)

def _insert_attribute_value(conn, node, node_id):
    value = node.value
    value_int = value_double = value_str = None
    if isinstance(value, int):
        value_int = value
    elif isinstance(value, float):
        value_double = value
    elif isinstance(value, basestring):
        value_str = value

    conn.execute(
        schema.attribute_values.insert(),
        node_id=node_id,
        value_str=value_str,
        value_int=value_int,
        value_double=value_double
    )

def _select_metadata(conn, file_id):
    qry = sql.select(
        [schema.nodes, schema.attribute_values],
        schema.nodes.c.file_id==file_id,
        from_obj=schema.nodes.outerjoin(schema.attribute_values),
        order_by=[
            schema.nodes.c.id.asc()
        ]
    )

    meta = oh5.Metadata()

    nodes = {}

    result = conn.execute(qry)
    row = result.fetchone()
    nodes[row[schema.nodes.c.id]] = meta.root()

    for row in result:
        node_id = row[schema.nodes.c.id]
        parent_id = row[schema.nodes.c.parent_id]
        parent = nodes[parent_id]
        node = _create_node_from_row(row)
        parent.add_child(node)
        nodes[node_id] = node
    
    return meta
 
def _create_node_from_row(row):
    type_ = row[schema.nodes.c.type]
    name = row[schema.nodes.c.name]
    if type_ == oh5.Attribute.type_id:
        node = oh5.Attribute(name, None)
        node.value = row[schema.attribute_values.c.value_str]
        if node.value is None:
            node.value = row[schema.attribute_values.c.value_int]
        if node.value is None:
            node.value = row[schema.attribute_values.c.value_double]
        return node
    elif type_ == oh5.Group.type_id:
        return oh5.Group(name)
    elif type_ == oh5.Dataset.type_id:
        return oh5.Dataset(name)
    else:
        raise RuntimeError("unhandled node type: %s" % type_)

def _has_file_by_hash_and_source(conn, hash, source_id):
    return conn.execute(
        sql.select(
            [sql.literal(True)],
            sql.and_(
                schema.files.c.hash==hash,
                schema.files.c.source_id==source_id
            )
        )
    ).scalar()