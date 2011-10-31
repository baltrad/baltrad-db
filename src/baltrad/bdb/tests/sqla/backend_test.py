import os
import sqlite3
from tempfile import NamedTemporaryFile
import uuid

from nose.tools import eq_, ok_, raises

from sqlalchemy import engine, event

from baltrad.bdb.sqla import schema
from baltrad.bdb.sqla.backend import SqlAlchemyBackend
from baltrad.bdb.backend import DuplicateEntry

from baltrad.bdb.oh5.meta import Metadata, HlHdfMetadataWriter, Source

def force_sqlite_foreign_keys(dbapi_con, con_record):
    if (isinstance(dbapi_con, sqlite3.Connection)):
        dbapi_con.execute("pragma foreign_keys=ON")

class TestSqlAlchemyBackendItest(object):
    engine = None
    backend = None

    sources = [
        Source({"_name": "eesur", "NOD": "eesur", "PLC": "Syrgavere"}),
        Source({"_name": "eehar", "NOD": "eehar", "PLC": "Harku"})
    ]


    @classmethod
    def _insert_sources(cls):
        conn = cls.engine.connect()
        for src in cls.sources:
            source = Source(dict(src))
            name = source.pop("_name")
            
            source_id = conn.execute(
                schema.sources.insert(),
                name=name,
            ).inserted_primary_key[0]
  
            kvs = []
            for k, v in source.iteritems():
                kvs.append({
                    "source_id": source_id,
                    "key": k,
                    "value": v,
                })
  
            conn.execute(
                schema.source_kvs.insert(),
                kvs
            )

    @classmethod
    def setUpClass(cls):
        url = os.environ.get("BDB_TEST_DB", "sqlite:///:memory:")
        cls.engine = engine.create_engine(url)
        event.listen(cls.engine, "connect", force_sqlite_foreign_keys)
        schema.meta.drop_all(cls.engine)
        schema.meta.create_all(cls.engine)
        cls._insert_sources()
        cls.backend = SqlAlchemyBackend(cls.engine)
    
    @classmethod
    def tearDownClass(cls):
        #schema.meta.drop_all(cls.engine)
        pass
    
    def tearDown(self):
        self.engine.execute(schema.files.delete())

    @staticmethod
    def create_metadata(what_object, what_date, what_time, what_source):
        meta = Metadata()
        meta.add_group("/what")
        meta.add_attribute("/what/object", "pvol")
        meta.add_attribute("/what/date", "20000131")
        meta.add_attribute("/what/time", "131415")
        meta.add_attribute("/what/source", "NOD:eesur")
        return meta
    
    def test_get_source_by_id(self):
        source = self.backend.get_source_by_id(1)
        eq_(3, len(source))
        eq_("eesur", source["NOD"])
        eq_("eesur", source.name)
        eq_("Syrgavere", source["PLC"])

    def test_get_source_id(self):
        source = {"NOD": "eesur"}
        eq_(1, self.backend.get_source_id(source))
    
    def test_store_file(self):
        meta = self.create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = NamedTemporaryFile()
        writer = HlHdfMetadataWriter()
        writer.write(meta, h5file.name)

        stored_meta = self.backend.store_file(h5file.name)
        ok_(stored_meta.bdb_uuid)
        ok_(stored_meta.bdb_metadata_hash)
        ok_(stored_meta.bdb_file_size)
        ok_(stored_meta.bdb_stored_date)
        ok_(stored_meta.bdb_stored_time)
        eq_("NOD:eesur,PLC:Syrgavere,_name:eesur", stored_meta.bdb_source)
    
    @raises(DuplicateEntry)
    def test_store_file_duplicate(self):
        meta = self.create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = NamedTemporaryFile()
        writer = HlHdfMetadataWriter()
        writer.write(meta, h5file.name)

        self.backend.store_file(h5file.name)
        self.backend.store_file(h5file.name)
    
    def test_get_file_metadata_nx(self):
        uuid_ = uuid.UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8")
        eq_(None, self.backend.get_file_metadata(uuid_))
    
    def test_get_file_metadata(self):
        meta = self.create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = NamedTemporaryFile()
        writer = HlHdfMetadataWriter()
        writer.write(meta, h5file.name)

        meta = self.backend.store_file(h5file.name)

        stored_meta = self.backend.get_file_metadata(meta.bdb_uuid)
        ok_(stored_meta, "no metadata returned")
        eq_(meta.what_object, stored_meta.what_object)
        eq_(meta.what_date, stored_meta.what_date)
        eq_(meta.what_time, stored_meta.what_time)
        eq_(meta.bdb_uuid, stored_meta.bdb_uuid)
        eq_(meta.bdb_file_size, stored_meta.bdb_file_size)
        eq_(meta.bdb_source, stored_meta.bdb_source)
        eq_(meta.bdb_stored_date, stored_meta.bdb_stored_date)
        eq_(meta.bdb_stored_time, stored_meta.bdb_stored_time)
    
    def test_get_file(self):
        meta = self.create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = NamedTemporaryFile()
        writer = HlHdfMetadataWriter()
        writer.write(meta, h5file.name)

        meta = self.backend.store_file(h5file.name)
        expected = open(h5file.name, "r").read()
        eq_(expected, self.backend.get_file(meta.bdb_uuid),
            "file content mismatch")
    
    def test_get_file_nx(self):
        uuid_ = uuid.UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8")
        eq_(None, self.backend.get_file(uuid_))
    
    def test_remove_file_nx(self):
        uuid_ = uuid.UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8")
        eq_(False, self.backend.remove_file(uuid_))
    
    def test_remove_file(self):
        meta = self.create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = NamedTemporaryFile()
        writer = HlHdfMetadataWriter()
        writer.write(meta, h5file.name)

        stored_meta = self.backend.store_file(h5file.name)

        eq_(True, self.backend.remove_file(stored_meta.bdb_uuid))
    
    def test_get_sources(self):
        sources = self.backend.get_sources()
        eq_(2, len(sources))
        ok_(self.sources[0] in sources)
        ok_(self.sources[1] in sources)
