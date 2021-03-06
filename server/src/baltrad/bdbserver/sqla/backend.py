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

from __future__ import absolute_import

import contextlib
import datetime
import logging
import os
import stat
import time
import uuid
import sys
from sys import path
if sys.version_info > (3,):
    long = int
    basestring = str

import migrate.versioning.api
import migrate.versioning.repository
from migrate import exceptions as migrateexc
from sqlalchemy import engine, event, exc as sqlexc, sql

from baltrad.bdbcommon import oh5
from baltrad.bdbserver import backend
from baltrad.bdbserver.sqla import filter, query, schema, storage
from sqlalchemy.sql.functions import GenericFunction as func
from sqlalchemy.exc import IntegrityError

logger = logging.getLogger("baltrad.bdbserver.sqla")

MIGRATION_REPO_PATH = os.path.join(os.path.dirname(__file__), "migrate")

def force_sqlite_foreign_keys(dbapi_con, con_record):
    try:
        import sqlite3
    except ImportError:
        # built without sqlite support
        return

    if isinstance(dbapi_con, sqlite3.Connection):
        dbapi_con.execute("pragma foreign_keys=ON")

def psql_set_extra_float_digits(dbapi_con, con_record):
    cursor = dbapi_con.cursor()
    cursor.execute("SET extra_float_digits=2")
    dbapi_con.commit()

class SqlAlchemyBackend(backend.Backend):
    """A backend using sqlalchemy to store metadata in a relational database
    and physical files either in the filesystem or in the database.

    :param engine_or_url: an SqlAlchemy engine or a database url
    :param storage: a `~.storage.FileStorage` instance to use.
    """
    def __init__(self, engine_or_url, storage, poolsize=10):
        if isinstance(engine_or_url, basestring):
            self._engine = engine.create_engine(engine_or_url, echo=False, pool_size=poolsize)
        else:
            self._engine = engine_or_url
        self._hasher = oh5.MetadataHasher()
        self._storage = storage
        
        if self._engine.driver == "pysqlite":
            event.listen(self._engine, "connect", force_sqlite_foreign_keys)
        if self._engine.name == "postgresql":
            event.listen(self._engine, "connect", psql_set_extra_float_digits)
        
        self._source_manager = SqlAlchemySourceManager(self)
        self._filter_manager = filter.SqlAlchemyFilterManager(self)
    
    @property
    def driver(self):
        """database driver name
        """
        return self._engine.driver
            
    @classmethod
    def from_conf(cls, conf):
        """create an instance from configuration

        parameters are looked up under *'baltrad.bdb.server.backend.sqla.'*.
        """
        fconf = conf.filter("baltrad.bdb.server.backend.sqla.")
        storage_type = fconf.get("storage.type", default="db")
        pool_size = fconf.get_int("pool_size", default=10)
        
        return SqlAlchemyBackend(
            conf["baltrad.bdb.server.backend.sqla.uri"],
            storage=storage.FileStorage.impl_from_conf(storage_type, conf),
            poolsize=pool_size
        )

    def store_file(self, path):
        st = time.time()
        meta = self.metadata_from_file(path)
        metadataTime = time.time()
        try:
            self._storage.store(self, meta, path)
            storageTime = time.time()
            logger.debug("Stored file with uuid: %s. hash: %s", meta.bdb_uuid, meta.bdb_metadata_hash)
            logger.info("sqla.backend.store_file: Metadata extraction time %d ms, storage time %d ms"%(int((metadataTime-st)*1000), int((storageTime-st)*1000)))
        except IntegrityError as e:
            message = str(e)
            if "duplicate key" in message:
                logger.warn("File already added to database. uuid: %s, hash: %s. Exception caught: %s", meta.bdb_uuid, meta.bdb_metadata_hash, message.splitlines()[0])
                raise backend.DuplicateEntry()
            else:
                raise e

        return meta
    
    def query_file_metadata(self, path):
        return self.metadata_from_file(path, ignore_duplicate=True)
        
    def get_file(self, uuid):
        try:
            return self._storage.read(self, uuid);
        except storage.FileNotFound:
            return None
    
    def remove_files_by_count(self, limit, nritems):
        sqry = sql.select([schema.files.c.id,schema.files.c.uuid]).order_by(schema.files.c.id.desc()).offset(limit).alias("sqry")
        qry = sql.select([sqry.c.uuid]).order_by(sqry.c.id).limit(nritems)
        result = 0
        with self.get_connection() as conn:
            for row in conn.execute(qry):
                if self.remove_file(row[schema.files.c.uuid]):
                    result = result + 1
        return result
 
    def remove_files_by_age(self, age, nritems):
        sqry = sql.select([schema.files.c.id,schema.files.c.uuid])
        sqry = sqry.where((schema.files.c.what_date+schema.files.c.what_time) < age)
        sqry = sqry.order_by(schema.files.c.what_date+schema.files.c.what_time).limit(nritems)
        
        result = 0
        with self.get_connection() as conn:
            for row in conn.execute(sqry):
                if self.remove_file(row[schema.files.c.uuid]):
                    result = result + 1
        return result
    
    def file_count(self):
        qry = sql.select([sql.func.count(schema.files.c.id)])
        with self.get_connection() as conn:
            return conn.execute(qry).scalar()
    
    def get_file_metadata(self, uuid):
        qry = sql.select(
            [schema.files],
            schema.files.c.uuid==str(uuid)
        )

        with self.get_connection() as conn:
            row = conn.execute(qry).fetchone()
            if not row:
                return None

        file_id = row[schema.files.c.id]
        
        with self.get_connection() as conn:
            meta = _select_metadata(conn, file_id)

        meta.bdb_uuid = row[schema.files.c.uuid]
        meta.bdb_file_size = row[schema.files.c.size]
        source_id = row[schema.files.c.source_id]
        with self.get_connection() as conn:
            source = get_source_by_id(conn, source_id)
        meta.bdb_source = source.to_string()
        meta.bdb_source_name = source.name
        meta.bdb_stored_date = row[schema.files.c.stored_date]
        meta.bdb_stored_time = row[schema.files.c.stored_time]

        return meta
     
    def remove_file(self, uuid):
        try:
            self._storage.remove(self, uuid)
        except storage.FileNotFound:
            return False
        else:
            return True
    
    def remove_all_files(self):
        qry = sql.select(
            [schema.files.c.uuid]
        )
        with self.get_connection() as conn:
            for row in conn.execute(qry):
                self.remove_file(row[schema.files.c.uuid])

    def file_source(self, uuid):
        qry = sql.select(
            [schema.source_kvs],
            from_obj=[schema.files.join(schema.sources).join(schema.source_kvs)],
        )
        qry = qry.where(schema.files.c.uuid==str(uuid))

        d = {}
        with self.get_connection() as conn:
            result = conn.execute(qry).fetchall()
            for row in result:
                key = row[schema.source_kvs.key]
                value = row[schema.source_kvs.value]
                d[key] = value
        return d
    
    def get_connection(self):
        """get a context managed connection to the database
        """
        return contextlib.closing(self._engine.connect())
    
    def get_source_manager(self):
        return self._source_manager
    
    def get_filter_manager(self):
        return self._filter_manager

    def execute_file_query(self, qry):
        stmt = query.transform_file_query(qry)
        r = []
        with self.get_connection() as conn:
            st = time.time()
            result = conn.execute(stmt).fetchall()
            logger.info("sqla.backend.execute_file_query: Took %d ms"%(int((time.time()-st)*1000)))
            for row in result:
                r.append({"uuid" : row[schema.files.c.uuid]})
        return r
    
    def execute_attribute_query(self, qry):
        stmt = query.transform_attribute_query(qry)
        r = []
        
        with self.get_connection() as conn:
            st = time.time()
            result = conn.execute(stmt)
            logger.info("sqla.backend.execute_attribute_query: Took %d ms"%(int((time.time()-st)*1000)))
            for row in result.fetchall():
                r.append(dict(zip(result.keys(), row)))
        return r
    
    def is_operational(self):
        required_tables = [table for table in schema.meta.sorted_tables]
        required_tables.remove(schema.file_data)

        with self.get_connection() as conn:
            for table in required_tables:
                if not table.exists(conn):
                    print("%s does not exist"%table.name)
                    return False
        return True
    
    def create(self):
        repo = migrate.versioning.repository.Repository(MIGRATION_REPO_PATH)
        migrate.versioning.api.version_control(self._engine, repo)
        migrate.versioning.api.upgrade(self._engine, repo)
    
    def drop(self):
        repo = migrate.versioning.repository.Repository(MIGRATION_REPO_PATH)
        try:
            migrate.versioning.api.downgrade(self._engine, repo, 0)
            migrate.versioning.api.drop_version_control(self._engine, repo)
        except migrateexc.DatabaseNotControlledError:
            pass
    
    def upgrade(self):
        repo = migrate.versioning.repository.Repository(MIGRATION_REPO_PATH)

        # try setting up version control for the databases created before
        # we started using sqlalchemy-migrate
        try:
            migrate.versioning.api.version_control(self._engine, repo, version=1)
        except migrateexc.DatabaseAlreadyControlledError:
            pass

        migrate.versioning.api.upgrade(self._engine, repo)
    
    def metadata_from_file(self, path, ignore_duplicate=False):
        meta = oh5.Metadata.from_file(path)

        metadata_hash = self._hasher.hash(meta)
        with self.get_connection() as conn:
            if meta.what_source == None:
                raise LookupError("no source in metadata")
            source_id = get_source_id(conn, meta.source())
            if not source_id:
                raise LookupError("failed to look up source for " +
                                  meta.source().to_string())
            if _has_file_by_hash_and_source(conn, metadata_hash, source_id) and ignore_duplicate == False:
                logger.warn("File with hash %s and source_id %s already existing in database." % (metadata_hash, source_id))
                raise backend.DuplicateEntry()

        meta.bdb_uuid = str(uuid.uuid4())
        meta.bdb_file_size = os.stat(path)[stat.ST_SIZE]
        meta.bdb_metadata_hash = metadata_hash
        stored_timestamp = datetime.datetime.utcnow()
        meta.bdb_stored_date = stored_timestamp.date()
        meta.bdb_stored_time = stored_timestamp.time()
        with self.get_connection() as conn:
            source = get_source_by_id(conn, source_id)
            
        # We must add file information to the metadata
        msources = meta.source()
        for k in msources.keys():
            if not source.has_key(k):
                source[k] = msources[k]
        meta.bdb_source = source.to_string()
        meta.bdb_source_name = source.name
        return meta

    def insert_metadata(self, conn, metadata):
        # XXX: source_id has actually been fetched before
        source_id = get_source_id_by_name(conn, metadata.bdb_source_name)
        file_id = insert_file(conn, metadata, source_id)
        associate_what_source(conn, file_id, metadata.source())
        insert_metadata(conn, metadata, file_id)
        return file_id 
    
    def delete_metadata(self, conn, uuid):
        qry = schema.files.delete(
            schema.files.c.uuid==str(uuid)
        )
        return bool(conn.execute(qry).rowcount)

    # This is a method that only should be called from a script so don't add it to rest-api.
    # 
    def change_storage(self, conf, src_type, tgt_type):
        if src_type == tgt_type:
            raise AttributeError("Can not specify same storage type for source and target (%s)"%src_type)
        
        n_files = self.file_count()
        src_storage=storage.FileStorage.impl_from_conf(src_type, conf)
        tgt_storage=storage.FileStorage.impl_from_conf(tgt_type, conf)
        print("Merging %d files from %s to %s"%(n_files, src_type, tgt_type))
        print("")
        
        n_migrated = 0
        n_failed = 0
        qry = sql.select(
            [schema.files.c.uuid]
        )
        with self.get_connection() as conn:
            for row in conn.execute(qry):
                uuid = row[schema.files.c.uuid]
                try:
                    obj = src_storage.read(self, uuid)
                    tgt_storage.store_file(self, uuid, obj)
                    src_storage.remove_file(self, uuid)
                    n_migrated = n_migrated + 1
                except storage.FileNotFound:
                    n_failed = n_failed + 1
                
                n_total = n_migrated + n_failed
                if n_total > 0 and n_total % 1000 == 0:
                    print("Migrated %d/%d files"%(n_total, n_files))
        print("Summary:")
        print("Migrated from %s to %s"%(src_type, tgt_type))
        print("Total number of files: %d"%(n_migrated + n_failed))
        print("Migrated: %d"%n_migrated)
        print("Failed: %d"%n_failed)
        
class SqlAlchemySourceManager(backend.SourceManager):
    def __init__(self, backend):
        self._backend = backend

    def get_sources(self):
        qry = sql.select(
            [schema.sources, schema.source_kvs],
            from_obj=schema.source_kvs.join(schema.sources),
            order_by=[sql.asc(schema.sources.c.name)]
        )

        result = []

        source = None

        with self.get_connection() as conn:
            for row in conn.execute(qry):
                if not source:
                    source = oh5.Source(row["name"])
                if source.name != row["name"]:
                    result.append(source)
                    source = oh5.Source(row["name"])
                source[row["key"]] = row["value"]
                source.parent = row["parent"]
        if source:
            result.append(source)
        return result
    
    def get_source(self, name):
        if not name:
            raise ValueError("name must not be None")

        qry = sql.select(
            [schema.sources, schema.source_kvs],
            from_obj=schema.source_kvs.join(schema.sources),
            order_by=[sql.asc(schema.sources.c.name)]
        )
        qry = qry.where(schema.sources.c.name==name)

        result = []

        source = None

        with self.get_connection() as conn:
            for row in conn.execute(qry):
                if not source:
                    source = oh5.Source(row["name"])
                if source.name != row["name"]:
                    result.append(source)
                    source = oh5.Source(row["name"])
                source[row["key"]] = row["value"]
                source.parent = row["parent"]
        if source:
            result.append(source)

        if len(result) == 0:
            raise LookupError("Could not find source with name: %s"%name)
        
        return result[0]
    
    def add_source(self, source):
        with self.get_connection() as conn:
            try:
                source_id = conn.execute(
                    schema.sources.insert(),
                    name=source.name,
                    parent=source.parent
                ).inserted_primary_key[0]
            except sqlexc.IntegrityError:
                raise backend.DuplicateEntry()
        
            insert_source_values(conn, source_id, source)
        return source_id
    
    def update_source(self, source):
        with self.get_connection() as conn:
            source_id = get_source_id_by_name(conn, source.name)
            if not source_id:
                raise LookupError("source '%s' not found" % source.name)

            with conn.begin():
                conn.execute(
                    schema.source_kvs.delete(
                        schema.source_kvs.c.source_id==source_id
                    )
                )
                insert_source_values(conn, source_id, source)
            
    def remove_source(self, name):
        with self.get_connection() as conn:
            try:
                affected_rows = conn.execute(
                    schema.sources.delete(
                        schema.sources.c.name==name
                    )
                ).rowcount
            except sqlexc.IntegrityError as e:
                raise backend.IntegrityError(str(e))
            else:
                return bool(affected_rows)
    
    def get_parent_sources(self):
        qry = sql.select(
            [schema.sources, schema.source_kvs],
            from_obj=schema.source_kvs.join(schema.sources),
            order_by=[sql.asc(schema.sources.c.name)]
        )
        qry = qry.where(schema.sources.c.parent==None)

        result = []

        source = None

        with self.get_connection() as conn:
            for row in conn.execute(qry):
                if not source:
                    source = oh5.Source(row["name"])
                if source.name != row["name"]:
                    result.append(source)
                    source = oh5.Source(row["name"])
                source[row["key"]] = row["value"]
        if source:
            result.append(source)
        return result  
          
    def get_sources_with_parent(self, parent):
        qry = sql.select(
            [schema.sources, schema.source_kvs],
            from_obj=schema.source_kvs.join(schema.sources),
            order_by=[sql.asc(schema.sources.c.name)]
        )
        qry = qry.where(schema.sources.c.parent==parent)

        result = []

        source = None

        with self.get_connection() as conn:
            for row in conn.execute(qry):
                if not source:
                    source = oh5.Source(row["name"])
                if source.name != row["name"]:
                    result.append(source)
                    source = oh5.Source(row["name"])
                source[row["key"]] = row["value"]
                source.parent = row["parent"]
        if source:
            result.append(source)
        return result

    def get_connection(self):
        return self._backend.get_connection()

def insert_file(conn, meta, source_id):
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

def merge_dictionaries(x, y):
    result = x.copy()
    result.update(y)
    return result

def insert_metadata(conn, meta, file_id):
    bulk = []
    for node in meta.iternodes():
        if node.path().startswith("/_bdb"):
            continue

        values = {"value_long": None, "value_string": None, "value_double": None, "value_boolean": None, "value_date": None, "value_time": None}
        if isinstance(node, oh5.Attribute):
            values = merge_dictionaries(values, _get_attribute_sql_values(node))

        bulk.append(
            dict(
                 file_id=file_id,
                 path=node.parent and node.parent.path() or "/",
                 name=node.name,
                 type=node.type_id,
                 **values
            )
        )
    conn.execute(schema.nodes.insert().values(bulk))

def _parse_date(datestr):
    if len(datestr) != 8 or not datestr.isdigit():
        return None
    try:
        tm = time.strptime(datestr, "%Y%m%d")
        return datetime.date(tm.tm_year, tm.tm_mon, tm.tm_mday)
    except ValueError:
        return None

def _parse_time(timestr):
    if len(timestr) != 6 or not timestr.isdigit():
        return None
    try:
        tm = time.strptime(timestr, "%H%M%S")
        return datetime.time(tm.tm_hour, tm.tm_min, tm.tm_sec)
    except ValueError:
        return None

def _parse_boolean(boolstr):
    if boolstr == "True":
        return True
    elif boolstr == "False":
        return False
    else:
        return None

def _get_attribute_sql_values(node):
    """get all possible sql values of an attribute value
    """
    values = {}
    value = node.value
    if isinstance(value, (long, int)):
        values["value_long"] = value
    elif isinstance(value, float):
        values["value_double"] = value
    elif isinstance(value, basestring):
        values["value_string"] = value
        date = _parse_date(value)
        if date:
            values["value_date"] = date
        time = _parse_time(value)
        if time:
            values["value_time"] = time
        boolean = _parse_boolean(value)
        if boolean is not None:
            values["value_boolean"] = boolean
    elif isinstance(value, list):
        logger.debug(
            "ignoring array attribute value at %s", node.path()
        )
    else:
        raise RuntimeError(
            "unhandled attribute value type: %s" % type(value)
        )
    return values

def create_nodename(row):
    name = row[schema.nodes.c.name]
    path = row[schema.nodes.c.path]
    result = path
    if not name is None:
        if path[-1] != "/":
            result = "%s/%s"%(result, name)
        else:
            result = "%s%s"%(result, name)
    return result

def _select_metadata(conn, file_id):
    qry = sql.select(
        [schema.nodes],
        schema.nodes.c.file_id==file_id,
        order_by=[
            schema.nodes.c.id.asc()
        ]
    )

    meta = oh5.Metadata()

    nodes = {}

    result = conn.execute(qry)
    row = result.fetchone()
    nodes[create_nodename(row)] = meta.root()

    for row in result:
        parent_path = row[schema.nodes.c.path]
        node = _create_node_from_row(row)
        if parent_path in nodes.keys():
            parent = nodes[parent_path]
            parent.add_child(node)
        nodes[create_nodename(row)] = node
    
    return meta
 
def _create_node_from_row(row):
    type_ = row[schema.nodes.c.type]
    name = row[schema.nodes.c.name]
    if type_ == oh5.Attribute.type_id:
        node = oh5.Attribute(name, None)
        node.value = row[schema.nodes.c.value_string]
        if node.value is None:
            node.value = row[schema.nodes.c.value_long]
        if node.value is None:
            node.value = row[schema.nodes.c.value_double]
        return node
    elif type_ == oh5.Group.type_id:
        return oh5.Group(name)
    elif type_ == oh5.Dataset.type_id:
        return oh5.Dataset(name)
    else:
        raise RuntimeError("unhandled node type: %s" % type_)

def _has_file_by_hash_and_source(conn, h, source_id):
    return conn.execute(
        sql.select(
            [sql.literal(True)],
            sql.and_(
                schema.files.c.hash==h,
                schema.files.c.source_id==source_id
            )
        )
    ).scalar()

def get_source_id(conn, source):
    where = sql.literal(False)
    keys = source.keys()
    ignoreORG=False
    if "ORG" in keys:
        if "WMO" in keys or "NOD" in keys or "RAD" in keys or "PLC" in keys or "WIGOS" in keys:
            ignoreORG=True
 
    for key, value in source.items():
        if ignoreORG and key == "ORG":
            continue
        where = sql.or_(
            where,
            sql.and_(
                schema.source_kvs.c.key==key,
                schema.source_kvs.c.value==value
            )
        )
 
    qry = sql.select(
        [schema.source_kvs.c.source_id, schema.source_kvs.c.key, schema.source_kvs.c.value],
        where,
        distinct=True
    )
     
    result = conn.execute(qry)
     
    source_id_matches = {}
    max_no_of_matches = 0
    best_match_id = None
    multiple_matches = False
    for row in result:
        source_id = row[schema.source_kvs.c.source_id]
        if not source_id in source_id_matches:
            source_id_matches[source_id] = 0
        row_key = row[schema.source_kvs.c.key]
        row_value = row[schema.source_kvs.c.value]
        for key, value in source.items():
            if ignoreORG and key == "ORG":
                continue
            elif key == row_key and value == row_value:
                source_id_matches[source_id] += 1
                if source_id_matches[source_id] > max_no_of_matches:
                    max_no_of_matches = source_id_matches[source_id]
                    best_match_id = source_id
                    multiple_matches = False
                elif source_id_matches[source_id] == max_no_of_matches:
                    multiple_matches = True
     
    if multiple_matches:
        logger.debug("Could not determine source due to multiple equally matching sources found for %s." % (str(source)))
        best_match_id = None
 
    return best_match_id

def get_source_id_by_name(conn, name):
    qry = sql.select(
        [schema.sources.c.id],
        schema.sources.c.name==name
    )

    return conn.execute(qry).scalar()

def get_source_by_id(conn, source_id):
    name_qry = sql.select(
        [schema.sources.c.name],
        schema.sources.c.id==source_id
    )

    kv_qry = sql.select(
        [schema.source_kvs],
        schema.source_kvs.c.source_id==source_id
    )
    
    source = oh5.Source()

    source.name = conn.execute(name_qry).scalar()
    for row in conn.execute(kv_qry).fetchall():
        source[row["key"]] = row["value"]

    return source

def insert_source_values(conn, source_id, source):
    kvs = []
    for k, v in source.items():
        kvs.append({
            "source_id": source_id,
            "key": k,
            "value": v,
        })
    
    conn.execute(
         schema.source_kvs.insert(),
         kvs
    )

def associate_what_source(conn, file_id, source):
    for key, value in source.items():
        kv_id = insert_what_source_kv(conn, key, value)
        conn.execute(
            schema.file_what_source.insert(),
            file_id=file_id, source_kv_id=kv_id
        )

def insert_what_source_kv(conn, key, value):
    kv_qry = sql.select(
        [schema.what_source_kvs.c.id],
        sql.and_(
            schema.what_source_kvs.c.key==key,
            schema.what_source_kvs.c.value==value
        )
    )
    kv_id = conn.execute(kv_qry).scalar()
    if not kv_id:
        kv_id = conn.execute(
            schema.what_source_kvs.insert(),
            key=key, value=value
        ).inserted_primary_key[0]
    return kv_id
