import tempfile
import uuid
import datetime

from nose.tools import eq_, ok_, raises
from nose.plugins.attrib import attr
from nose.plugins.skip import SkipTest

from baltrad.bdbserver.sqla import schema
from baltrad.bdbserver.sqla.backend import (
    get_source_id,
    get_source_by_id,
    SqlAlchemySourceManager,
)

from baltrad.bdbserver.backend import (
    DuplicateEntry,
    IntegrityError,
)

from baltrad.bdbcommon.oh5.meta import Metadata, Source
from baltrad.bdbcommon.oh5.io import HlHdfMetadataWriter 
from baltrad.bdbcommon.oh5.node import Attribute, Group

from . import get_backend

def create_metadata(what_object, what_date, what_time, what_source):
    meta = Metadata()
    meta.add_node("/", Group("what"))
    meta.add_node("/what", Attribute("object", what_object))
    meta.add_node("/what", Attribute("date", what_date))
    meta.add_node("/what", Attribute("time", what_time))
    meta.add_node("/what", Attribute("source", what_source))
    return meta

def write_metadata(meta):
    h5file = tempfile.NamedTemporaryFile()
    writer = HlHdfMetadataWriter()
    writer.write(meta, h5file.name)
    return h5file

class TestSqlAlchemyBackendItest(object):
    backend = None

    sources = [
        Source("eesur", values={"NOD": "eesur", "PLC": "Syrgavere"}),
        Source("eehar", values={"NOD": "eehar", "PLC": "Harku"})
    ]

    source_ids = []

    @classmethod
    def setup_class(cls):
        cls.backend = get_backend()
        if not cls.backend:
            return

        for src in cls.sources:
            cls.source_ids.append(
                cls.backend.get_source_manager().add_source(src)
            )
    
    @classmethod
    def teardown_class(cls):
        if not cls.backend:
            return

        with cls.backend.get_connection() as conn:
            conn.execute(schema.sources.delete())
    
    def setup(self):
        if not self.backend:
            raise SkipTest("no backend defined")

    def tearDown(self):
        with self.backend.get_connection() as conn:
            conn.execute(schema.files.delete())
    
    @attr("dbtest")
    def test_get_source_by_id(self):
        with self.backend.get_connection() as conn:
            source = get_source_by_id(conn, self.source_ids[0])
            eq_(self.sources[0], source)

    @attr("dbtest")
    def test_get_source_id(self):
        source = {"NOD": "eesur"}
        with self.backend.get_connection() as conn:
            eq_(self.source_ids[0], get_source_id(conn, source))
    
    @attr("dbtest")
    def test_store_file(self):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        stored_meta = self.backend.store_file(h5file.name)
        ok_(stored_meta.bdb_uuid)
        ok_(stored_meta.bdb_metadata_hash)
        ok_(stored_meta.bdb_file_size)
        ok_(stored_meta.bdb_stored_date)
        ok_(stored_meta.bdb_stored_time)
        eq_("NOD:eesur,PLC:Syrgavere", stored_meta.bdb_source)
        eq_("eesur", stored_meta.bdb_source_name)
    
    @attr("dbtest")
    @raises(DuplicateEntry)
    def test_store_file_duplicate(self):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        self.backend.store_file(h5file.name)
        self.backend.store_file(h5file.name)
    
    @attr("dbtest")
    def test_get_file_metadata_nx(self):
        uuid_ = uuid.UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8")
        eq_(None, self.backend.get_file_metadata(uuid_))
    
    @attr("dbtest")
    def test_get_file_metadata(self):
        float_max = 1.7976931348623157e+308
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        meta.add_node("/", Attribute("double", float_max))
        h5file = write_metadata(meta)

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
        eq_(float_max, stored_meta.node("/double").value)

    @attr("dbtest")
    def test_remove_files_by_count_1(self):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        self.backend.store_file(h5file1.name)
        self.backend.store_file(h5file2.name)
        self.backend.store_file(h5file3.name)
        self.backend.store_file(h5file4.name)

        eq_(4, self.backend.file_count())
    
        result = self.backend.remove_files_by_count(3,2)
        eq_(1, result)
        eq_(3, self.backend.file_count())

    @attr("dbtest")
    def test_remove_files_by_count_2(self):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        self.backend.store_file(h5file1.name)
        self.backend.store_file(h5file2.name)
        self.backend.store_file(h5file3.name)
        self.backend.store_file(h5file4.name)

        eq_(4, self.backend.file_count())
    
        result = self.backend.remove_files_by_count(4,2)
        eq_(0, result)
        eq_(4, self.backend.file_count())

    @attr("dbtest")
    def test_remove_files_by_count_3(self):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        self.backend.store_file(h5file1.name)
        self.backend.store_file(h5file2.name)
        self.backend.store_file(h5file3.name)
        self.backend.store_file(h5file4.name)

        eq_(4, self.backend.file_count())
    
        result = self.backend.remove_files_by_count(1,2)
        eq_(2, result)
        eq_(2, self.backend.file_count())
    
    @attr("dbtest")
    def test_remove_files_by_count_4(self):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))
        h5file5 = write_metadata(create_metadata("pvol", "20000131", "131515", "NOD:eesur"))
        h5file6 = write_metadata(create_metadata("pvol", "20000131", "131530", "NOD:eesur"))
        h5file7 = write_metadata(create_metadata("pvol", "20000131", "131545", "NOD:eesur"))
        h5file8 = write_metadata(create_metadata("pvol", "20000131", "131600", "NOD:eesur"))
        h5file9 = write_metadata(create_metadata("pvol", "20000131", "131615", "NOD:eesur"))
        h5file10 = write_metadata(create_metadata("pvol", "20000131", "131630", "NOD:eesur"))

        self.backend.store_file(h5file1.name)
        self.backend.store_file(h5file2.name)
        self.backend.store_file(h5file3.name)
        self.backend.store_file(h5file4.name)
        self.backend.store_file(h5file5.name)
        self.backend.store_file(h5file6.name)
        self.backend.store_file(h5file7.name)
        self.backend.store_file(h5file8.name)
        self.backend.store_file(h5file9.name)
        self.backend.store_file(h5file10.name)

        eq_(10, self.backend.file_count())
        
        result = self.backend.remove_files_by_count(10,2)
        eq_(0, result)
        eq_(10, self.backend.file_count())
    
        result = self.backend.remove_files_by_count(8,10)
        eq_(2, result)
        eq_(8, self.backend.file_count())

        result = self.backend.remove_files_by_count(6,1)
        eq_(1, result)
        eq_(7, self.backend.file_count())

        result = self.backend.remove_files_by_count(6,1)
        eq_(1, result)
        eq_(6, self.backend.file_count())

        result = self.backend.remove_files_by_count(6,1)
        eq_(0, result)
        eq_(6, self.backend.file_count())

        result = self.backend.remove_files_by_count(0,1000)
        eq_(6, result)
        eq_(0, self.backend.file_count())
    
    @attr("dbtest")
    def test_remove_files_by_age_1(self):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        self.backend.store_file(h5file1.name)
        self.backend.store_file(h5file2.name)
        self.backend.store_file(h5file3.name)
        self.backend.store_file(h5file4.name)

        eq_(4, self.backend.file_count())
    
        result = self.backend.remove_files_by_age(datetime.datetime(2000,1,31,13,14,30,0),2)
        eq_(1, result)
        eq_(3, self.backend.file_count())

    @attr("dbtest")
    def test_remove_files_by_age_2(self):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        self.backend.store_file(h5file1.name)
        self.backend.store_file(h5file2.name)
        self.backend.store_file(h5file3.name)
        self.backend.store_file(h5file4.name)

        eq_(4, self.backend.file_count())
    
        result = self.backend.remove_files_by_age(datetime.datetime(2000,1,31,13,14,0,0),2)
        eq_(0, result)
        eq_(4, self.backend.file_count())

    @attr("dbtest")
    def test_remove_files_by_age_3(self):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        self.backend.store_file(h5file1.name)
        self.backend.store_file(h5file2.name)
        self.backend.store_file(h5file3.name)
        self.backend.store_file(h5file4.name)

        eq_(4, self.backend.file_count())
    
        result = self.backend.remove_files_by_age(datetime.datetime(2000,1,31,13,14,45,0),2)
        eq_(2, result)
        eq_(2, self.backend.file_count())

    @attr("dbtest")
    def test_remove_files_by_age_4(self):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        self.backend.store_file(h5file1.name)
        self.backend.store_file(h5file2.name)
        self.backend.store_file(h5file3.name)
        self.backend.store_file(h5file4.name)

        eq_(4, self.backend.file_count())
    
        result = self.backend.remove_files_by_age(datetime.datetime(2000,1,31,13,14,30,0),2)
        eq_(1, result)
        eq_(3, self.backend.file_count())

        result = self.backend.remove_files_by_age(datetime.datetime(2000,1,31,13,15,00,0),2)
        eq_(2, result)
        eq_(1, self.backend.file_count())

        result = self.backend.remove_files_by_age(datetime.datetime(2000,1,31,13,16,00,0),2)
        eq_(1, result)
        eq_(0, self.backend.file_count())
    
    @attr("dbtest")
    def test_get_file(self):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        meta = self.backend.store_file(h5file.name)
        expected = open(h5file.name, "r").read()
        eq_(expected, self.backend.get_file(meta.bdb_uuid),
            "file content mismatch")

    
    @attr("dbtest")
    def test_file_count(self):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)
        meta2 = create_metadata("pvol", "20000131", "131430", "NOD:eesur")
        h5file2 = write_metadata(meta2)
        
        meta = self.backend.store_file(h5file.name)
        meta2 = self.backend.store_file(h5file2.name)

        eq_(2, self.backend.file_count())
    
    @attr("dbtest")
    def test_get_file_nx(self):
        uuid_ = uuid.UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8")
        eq_(None, self.backend.get_file(uuid_))
    
    @attr("dbtest")
    def test_remove_file_nx(self):
        uuid_ = uuid.UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8")
        eq_(False, self.backend.remove_file(uuid_))
    
    @attr("dbtest")
    def test_remove_file(self):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        stored_meta = self.backend.store_file(h5file.name)

        eq_(True, self.backend.remove_file(stored_meta.bdb_uuid))
    
    @attr("dbtest")
    def test_remove_files(self):        
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        meta = create_metadata("pvol", "20000131", "131416", "NOD:eesur")
        h5file = write_metadata(meta)

        self.backend.store_file(h5file.name)

        self.backend.remove_all_files()

        with self.backend.get_connection() as conn:
            delete_count = conn.execute(schema.files.count()).scalar()
        eq_(0, delete_count) 
    
class TestSqlAlchemySourceManager(object):
    backend = None

    @classmethod
    def setup_class(cls):
        cls.backend = get_backend()

    def setup(self):
        if not self.backend:
            raise SkipTest("no backend defined")
        self.source_manager = SqlAlchemySourceManager(self.backend)
    
    def tearDown(self):
        with self.backend.get_connection() as conn:
            conn.execute(schema.files.delete())
            conn.execute(schema.sources.delete())
    
    @attr("dbtest")
    def test_add_source(self):
        source = Source("foo", values={"bar": "baz"})
        self.source_manager.add_source(source)

        sources = self.source_manager.get_sources()
        eq_(1, len(sources))
        eq_(source, sources[0])
    
    @attr("dbtest")
    @raises(DuplicateEntry)
    def test_add_source_duplicate(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("foo", values={"qwe": "asd"})

        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)

    @attr("dbtest")
    def test_update_source(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("foo", values={"asd": "qaz"})
        self.source_manager.add_source(source1)

        self.source_manager.update_source(source2)

        sources = self.source_manager.get_sources()
        eq_(1, len(sources))
        eq_(source2, sources[0])
    
    @attr("dbtest")
    @raises(LookupError)
    def test_update_source_not_found(self):
        source1 = Source("qwe", values={"bar": "baz"})
        self.source_manager.update_source(source1)
    
    @attr("dbtest")
    def test_remove_source(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("qwe", values={"asd": "qaz"})
        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)
        
        eq_(True, self.source_manager.remove_source("foo"))

        sources = self.source_manager.get_sources()
        eq_(1, len(sources))
        eq_(source2, sources[0])
    
    @attr("dbtest")
    def test_remove_source_not_found(self):
        eq_(False, self.source_manager.remove_source("foo"))

    @attr("dbtest")
    @raises(IntegrityError)
    def test_remove_source_files_attached(self):
        source1 = Source("foo", values={"bar": "baz"})
        self.source_manager.add_source(source1)

        meta = create_metadata("pvol", "20000131", "131415", "bar:baz")
        h5file = write_metadata(meta)

        self.backend.store_file(h5file.name)

        self.source_manager.remove_source("foo")

    @attr("dbtest")
    def test_get_sources(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("bar", values={"qwe": "asd"})
        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)

        sources = self.source_manager.get_sources()
        eq_(2, len(sources))
        # in alphabetical order by name
        eq_(source2, sources[0])
        eq_(source1, sources[1])
    
    @attr("dbtest")
    def test_get_sources_empty(self):
        sources = self.source_manager.get_sources()
        eq_(0, len(sources))
