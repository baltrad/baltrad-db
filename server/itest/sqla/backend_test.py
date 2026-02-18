import tempfile
import uuid
import datetime

#from nose.tools import eq_, ok_, raises
#from nose.plugins.attrib import attr
#from nose.plugins.skip import SkipTest
import pytest

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
from .conftest import bdb_backend
#from . import get_backend

from sqlalchemy import func, sql

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

@pytest.mark.dbtest
class TestSqlAlchemyBackendItest(object):
    backend = None

    sources = [
        Source("eesur", values={"NOD": "eesur", "PLC": "Syrgavere"}),
        Source("eehar", values={"NOD": "eehar", "PLC": "Harku"}),
        Source("dkaal", values={"WMO": "00000", "NOD": "dkaal", "PLC": "Aalborg"}),
        Source("sebaa", values={"WMO": "00000", "NOD": "sebaa", "PLC": "Balsta"}),
        Source("sekrn", values={"WIGOS": "0-20000-0-2032", "NOD": "sekrn", "PLC": "Kiruna"}),
        Source("sekaa", values={"WIGOS": "0-20000-0-2666","WMO": "02666", "NOD": "sekaa", "PLC": "Karlskrona"})
    ]

    source_ids = []

    @pytest.fixture(scope="class", autouse=True)
    def setup_class_fixture(self, bdb_backend):
        if not bdb_backend:
            pytest.skip("no backend defined")

        for src in self.sources:
            self.source_ids.append(
                bdb_backend.get_source_manager().add_source(src)
            )

        yield
        
        with bdb_backend.get_connection() as conn:
            conn.execute(schema.file_what_source.delete())
            conn.execute(schema.nodes.delete())
            conn.execute(schema.files.delete())
            conn.execute(schema.sources.delete())
            conn.commit()

    @pytest.fixture(autouse=True)
    def teardown(self, bdb_backend):
        with bdb_backend.get_connection() as conn:    
            conn.execute(schema.file_what_source.delete())
            conn.execute(schema.nodes.delete())
            conn.execute(schema.files.delete())
            conn.commit()

        yield

        with bdb_backend.get_connection() as conn:    
            conn.execute(schema.file_what_source.delete())
            conn.execute(schema.nodes.delete())
            conn.execute(schema.files.delete())
            conn.commit()

    @pytest.mark.dbtest
    def test_get_source_by_id(self, bdb_backend):
        with bdb_backend.get_connection() as conn:
            source = get_source_by_id(conn, self.source_ids[0])
            assert(self.sources[0]==source)

    @pytest.mark.dbtest
    def test_get_source_id(self, bdb_backend):
        source = {"NOD": "eesur"}
        with bdb_backend.get_connection() as conn:
            assert(self.source_ids[0]==get_source_id(conn, source))
            
    @pytest.mark.dbtest
    def test_get_source_id_multiple_wmo_matches(self, bdb_backend):
        source = {"WMO": "00000", "NOD": "sebaa"}
        with bdb_backend.get_connection() as conn:
            assert(self.source_ids[3] == get_source_id(conn, source))
    
    @pytest.mark.dbtest
    def test_store_file(self, bdb_backend):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        stored_meta = bdb_backend.store_file(h5file.name)
        assert(stored_meta.bdb_uuid)
        assert(stored_meta.bdb_metadata_hash)
        assert(stored_meta.bdb_file_size)
        assert(stored_meta.bdb_stored_date)
        assert(stored_meta.bdb_stored_time)
        assert("NOD:eesur,PLC:Syrgavere"==stored_meta.bdb_source)
        assert("eesur"==stored_meta.bdb_source_name)
    
    @pytest.mark.dbtest
    def test_store_file_duplicate(self, bdb_backend):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        bdb_backend.store_file(h5file.name)
        with pytest.raises(DuplicateEntry):
            bdb_backend.store_file(h5file.name)
    
    @pytest.mark.dbtest
    def test_get_file_metadata_nx(self, bdb_backend):
        uuid_ = uuid.UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8")
        assert(bdb_backend.get_file_metadata(uuid_) == None)
    
    @pytest.mark.dbtest
    def test_get_file_metadata(self, bdb_backend):
        float_max = 1.7976931348623157e+308
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        meta.add_node("/", Attribute("double", float_max))
        h5file = write_metadata(meta)

        meta = bdb_backend.store_file(h5file.name)

        stored_meta = bdb_backend.get_file_metadata(meta.bdb_uuid)
        assert(stored_meta), "no metadata returned"
        assert(stored_meta.what_object == meta.what_object)
        assert(stored_meta.what_date == meta.what_date)
        assert(stored_meta.what_time == meta.what_time)
        assert(stored_meta.bdb_uuid == meta.bdb_uuid)
        assert(stored_meta.bdb_file_size == meta.bdb_file_size)
        assert(stored_meta.bdb_source == meta.bdb_source)
        assert(stored_meta.bdb_stored_date == meta.bdb_stored_date)
        assert(stored_meta.bdb_stored_time == meta.bdb_stored_time)
        assert(stored_meta.node("/double").value == float_max )

    @pytest.mark.dbtest
    def test_query_file_metadata(self, bdb_backend):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        filemeta = bdb_backend.query_file_metadata(h5file.name)
        assert(filemeta.bdb_uuid)
        assert(filemeta.bdb_metadata_hash)
        assert(filemeta.bdb_file_size)
        assert(filemeta.bdb_stored_date)
        assert(filemeta.bdb_stored_time)
        assert(filemeta.bdb_source == "NOD:eesur,PLC:Syrgavere")
        assert(filemeta.bdb_source_name == "eesur")

        assert(bdb_backend.file_count() == 0)

    @pytest.mark.dbtest
    def test_remove_files_by_count_1(self, bdb_backend):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        bdb_backend.store_file(h5file1.name)
        bdb_backend.store_file(h5file2.name)
        bdb_backend.store_file(h5file3.name)
        bdb_backend.store_file(h5file4.name)

        assert(bdb_backend.file_count() == 4)
    
        result = bdb_backend.remove_files_by_count(3,2)
        assert(result == 1)
        assert(bdb_backend.file_count() == 3)

    @pytest.mark.dbtest
    def test_remove_files_by_count_2(self, bdb_backend):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        bdb_backend.store_file(h5file1.name)
        bdb_backend.store_file(h5file2.name)
        bdb_backend.store_file(h5file3.name)
        bdb_backend.store_file(h5file4.name)

        assert(bdb_backend.file_count() == 4)
    
        result = bdb_backend.remove_files_by_count(4,2)
        assert(result == 0)
        assert(bdb_backend.file_count() == 4)

    @pytest.mark.dbtest
    def test_remove_files_by_count_3(self, bdb_backend):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        bdb_backend.store_file(h5file1.name)
        bdb_backend.store_file(h5file2.name)
        bdb_backend.store_file(h5file3.name)
        bdb_backend.store_file(h5file4.name)

        assert(bdb_backend.file_count() == 4)
    
        result = bdb_backend.remove_files_by_count(1,2)
        assert(result == 2)
        assert(bdb_backend.file_count() == 2)
    
    @pytest.mark.dbtest
    def test_remove_files_by_count_4(self, bdb_backend):
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

        bdb_backend.store_file(h5file1.name)
        bdb_backend.store_file(h5file2.name)
        bdb_backend.store_file(h5file3.name)
        bdb_backend.store_file(h5file4.name)
        bdb_backend.store_file(h5file5.name)
        bdb_backend.store_file(h5file6.name)
        bdb_backend.store_file(h5file7.name)
        bdb_backend.store_file(h5file8.name)
        bdb_backend.store_file(h5file9.name)
        bdb_backend.store_file(h5file10.name)

        assert(bdb_backend.file_count() == 10)
        
        result = bdb_backend.remove_files_by_count(10,2)
        assert(result == 0)
        assert(bdb_backend.file_count() == 10)
    
        result = bdb_backend.remove_files_by_count(8,10)
        assert(result == 2)
        assert(bdb_backend.file_count() == 8)

        result = bdb_backend.remove_files_by_count(6,1)
        assert(result == 1)
        assert(bdb_backend.file_count() == 7)

        result = bdb_backend.remove_files_by_count(6,1)
        assert(result == 1)
        assert(bdb_backend.file_count() == 6)

        result = bdb_backend.remove_files_by_count(6,1)
        assert(result == 0)
        assert(bdb_backend.file_count() == 6)

        result = bdb_backend.remove_files_by_count(0,1000)
        assert(result == 6)
        assert(bdb_backend.file_count() == 0)
    
    @pytest.mark.dbtest
    def test_remove_files_by_age_1(self, bdb_backend):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        bdb_backend.store_file(h5file1.name)
        bdb_backend.store_file(h5file2.name)
        bdb_backend.store_file(h5file3.name)
        bdb_backend.store_file(h5file4.name)

        assert(bdb_backend.file_count() == 4)
    
        result = bdb_backend.remove_files_by_age(datetime.datetime(2000,1,31,13,14,30,0),2)
        assert(result == 1)
        assert(bdb_backend.file_count() == 3)

    @pytest.mark.dbtest
    def test_remove_files_by_age_2(self, bdb_backend):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        bdb_backend.store_file(h5file1.name)
        bdb_backend.store_file(h5file2.name)
        bdb_backend.store_file(h5file3.name)
        bdb_backend.store_file(h5file4.name)

        assert(bdb_backend.file_count() == 4)
    
        result = bdb_backend.remove_files_by_age(datetime.datetime(2000,1,31,13,14,0,0),2)
        assert(result == 0)
        assert(bdb_backend.file_count() == 4)

    @pytest.mark.dbtest
    def test_remove_files_by_age_3(self, bdb_backend):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        bdb_backend.store_file(h5file1.name)
        bdb_backend.store_file(h5file2.name)
        bdb_backend.store_file(h5file3.name)
        bdb_backend.store_file(h5file4.name)

        assert(bdb_backend.file_count() == 4)
    
        result = bdb_backend.remove_files_by_age(datetime.datetime(2000,1,31,13,14,45,0),2)
        assert(result == 2)
        assert(bdb_backend.file_count() == 2)

    @pytest.mark.dbtest
    def test_remove_files_by_age_4(self, bdb_backend):
        h5file1 = write_metadata(create_metadata("pvol", "20000131", "131415", "NOD:eesur"))
        h5file2 = write_metadata(create_metadata("pvol", "20000131", "131430", "NOD:eesur"))
        h5file3 = write_metadata(create_metadata("pvol", "20000131", "131445", "NOD:eesur"))
        h5file4 = write_metadata(create_metadata("pvol", "20000131", "131500", "NOD:eesur"))

        bdb_backend.store_file(h5file1.name)
        bdb_backend.store_file(h5file2.name)
        bdb_backend.store_file(h5file3.name)
        bdb_backend.store_file(h5file4.name)

        assert(bdb_backend.file_count() == 4)
    
        result = bdb_backend.remove_files_by_age(datetime.datetime(2000,1,31,13,14,30,0),2)
        assert(result == 1)
        assert(bdb_backend.file_count() == 3)

        result = bdb_backend.remove_files_by_age(datetime.datetime(2000,1,31,13,15,00,0),2)
        assert(result == 2)
        assert(bdb_backend.file_count() == 1)

        result = bdb_backend.remove_files_by_age(datetime.datetime(2000,1,31,13,16,00,0),2)
        assert(result == 1)
        assert(bdb_backend.file_count() == 0)
    
    @pytest.mark.dbtest
    def test_get_file(self, bdb_backend):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        meta = bdb_backend.store_file(h5file.name)
        expected = open(h5file.name, "rb").read()
        assert(bdb_backend.get_file(meta.bdb_uuid) == expected)
    
    @pytest.mark.dbtest
    def test_file_count(self, bdb_backend):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)
        meta2 = create_metadata("pvol", "20000131", "131430", "NOD:eesur")
        h5file2 = write_metadata(meta2)
        
        meta = bdb_backend.store_file(h5file.name)
        meta2 = bdb_backend.store_file(h5file2.name)

        assert(bdb_backend.file_count() == 2)
    
    @pytest.mark.dbtest
    def test_get_file_nx(self, bdb_backend):
        uuid_ = uuid.UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8")
        assert(bdb_backend.get_file(uuid_) == None)
    
    @pytest.mark.dbtest
    def test_remove_file_nx(self, bdb_backend):
        uuid_ = uuid.UUID("6ba7b810-9dad-11d1-80b4-00c04fd430c8")
        assert(bdb_backend.remove_file(uuid_) == False)
    
    @pytest.mark.dbtest
    def test_remove_file(self, bdb_backend):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        stored_meta = bdb_backend.store_file(h5file.name)

        assert(bdb_backend.remove_file(stored_meta.bdb_uuid) == True)
    
    @pytest.mark.dbtest
    def test_remove_files(self, bdb_backend):        
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        meta = create_metadata("pvol", "20000131", "131416", "NOD:eesur")
        h5file = write_metadata(meta)

        bdb_backend.store_file(h5file.name)

        bdb_backend.remove_all_files()

        with bdb_backend.get_connection() as conn:
            delete_count = conn.execute(sql.select(func.count()).select_from(schema.files)).scalar_one()
        assert(delete_count == 0) 

@pytest.mark.dbtest
class TestSqlAlchemySourceManager(object):
    @pytest.fixture(autouse=True)
    def setup_test(self, bdb_backend):
        if not bdb_backend:
            pytest.skip("no backend defined")

        self.source_manager = SqlAlchemySourceManager(bdb_backend)

        yield

        with bdb_backend.get_connection() as conn:
            conn.execute(schema.files.delete())
            conn.execute(schema.sources.delete())
            conn.commit()
    
    @pytest.mark.dbtest
    def test_add_source(self, bdb_backend):
        source = Source("foo", values={"bar": "baz"})
        self.source_manager.add_source(source)

        sources = self.source_manager.get_sources()
        assert(len(sources) == 1)
        assert(sources[0] == source)
    
    @pytest.mark.dbtest
    def test_add_source_duplicate(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("foo", values={"qwe": "asd"})
        self.source_manager.add_source(source1)
        with pytest.raises(DuplicateEntry):
            self.source_manager.add_source(source2)

    @pytest.mark.dbtest
    def test_update_source(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("foo", values={"asd": "qaz"})
        self.source_manager.add_source(source1)

        self.source_manager.update_source(source2)

        sources = self.source_manager.get_sources()
        assert(len(sources == 1))
        assert(sources[0] == source2)
    
    @pytest.mark.dbtest
    def test_update_source_not_found(self):
        source1 = Source("qwe", values={"bar": "baz"})
        with pytest.raises(LookupError):
            self.source_manager.update_source(source1)
    
    @pytest.mark.dbtest
    def test_remove_source(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("qwe", values={"asd": "qaz"})
        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)
        
        assert(self.source_manager.remove_source("foo" == True))

        sources = self.source_manager.get_sources()
        assert(len(sources == 1))
        assert(sources[0] == source2)
    
    @pytest.mark.dbtest
    def test_remove_source_not_found(self):
        assert(self.source_manager.remove_source("foo" == False))

    @pytest.mark.dbtest
    def test_remove_source_files_attached(self):
        source1 = Source("foo", values={"bar": "baz"})
        self.source_manager.add_source(source1)

        meta = create_metadata("pvol", "20000131", "131415", "bar:baz")
        h5file = write_metadata(meta)

        bdb_backend.store_file(h5file.name)

        with pytest.raises(IntegrityError):
            self.source_manager.remove_source("foo")

    @pytest.mark.dbtest
    def test_get_sources(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("bar", values={"qwe": "asd"})
        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)

        sources = self.source_manager.get_sources()
        assert(len(sources) == 2)
        # in alphabetical order by name
        assert(sources[0] == source2)
        assert(sources[1] == source1)
        
    @pytest.mark.dbtest
    def test_get_source(self):
        source1 = Source("se", values={"bar": "baz"})
        source2 = Source("sekir", values={"qwe": "asd"}, parent="se")
        source3 = Source("pl", values={"bar": "boo"})
        source4 = Source("plika", values={"qwe": "moha"}, parent="pl")
        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)
        self.source_manager.add_source(source3)
        self.source_manager.add_source(source4)

        assert(self.source_manager.get_source("se") == source1)
        assert(self.source_manager.get_source("sekir") == source2)
        assert(self.source_manager.get_source("pl") == source3)
        assert(self.source_manager.get_source("plika") == source4)

    @pytest.mark.dbtest
    def test_get_source_not_found(self):
        source1 = Source("se", values={"bar": "baz"})

    @pytest.mark.dbtest
    def test_add_source_duplicate(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("foo", values={"qwe": "asd"})

        self.source_manager.add_source(source1)
        with pytest.raises(DuplicateEntry):
            self.source_manager.add_source(source2)

    @pytest.mark.dbtest
    def test_update_source(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("foo", values={"asd": "qaz"})
        self.source_manager.add_source(source1)

        self.source_manager.update_source(source2)

        sources = self.source_manager.get_sources()
        assert(len(sources) == 1)
        assert(sources[0] == source2)
    
    @pytest.mark.dbtest
    def test_update_source_not_found(self):
        source1 = Source("qwe", values={"bar": "baz"})
        with pytest.raises(LookupError):
            self.source_manager.update_source(source1)
    
    @pytest.mark.dbtest
    def test_remove_source(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("qwe", values={"asd": "qaz"})
        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)
        
        assert(self.source_manager.remove_source("foo") == True)

        sources = self.source_manager.get_sources()
        assert(len(sources) == 1)
        assert(sources[0] == source2)
    
    @pytest.mark.dbtest
    def test_remove_source_not_found(self):
        assert(self.source_manager.remove_source("foo") == False)

    @pytest.mark.dbtest
    def test_remove_source_files_attached(self, bdb_backend):
        source1 = Source("foo", values={"bar": "baz"})
        self.source_manager.add_source(source1)

        meta = create_metadata("pvol", "20000131", "131415", "bar:baz")
        h5file = write_metadata(meta)

        bdb_backend.store_file(h5file.name)
        with pytest.raises(IntegrityError):
            self.source_manager.remove_source("foo")

    @pytest.mark.dbtest
    def test_get_sources(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("bar", values={"qwe": "asd"})
        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)

        sources = self.source_manager.get_sources()
        assert(len(sources) == 2)
        # in alphabetical order by name
        assert(sources[0] == source2)
        assert(sources[1] == source1)
        
    @pytest.mark.dbtest
    def test_get_source(self):
        source1 = Source("se", values={"bar": "baz"})
        source2 = Source("sekir", values={"qwe": "asd"}, parent="se")
        source3 = Source("pl", values={"bar": "boo"})
        source4 = Source("plika", values={"qwe": "moha"}, parent="pl")
        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)
        self.source_manager.add_source(source3)
        self.source_manager.add_source(source4)

        assert(self.source_manager.get_source("se") == source1)
        assert(self.source_manager.get_source("sekir") == source2)
        assert(self.source_manager.get_source("pl") == source3)
        assert(self.source_manager.get_source("plika") == source4)

    @pytest.mark.dbtest
    def test_get_source_not_found(self):
        source1 = Source("se", values={"bar": "baz"})
        source2 = Source("sekir", values={"qwe": "asd"}, parent="se")
        source3 = Source("pl", values={"bar": "boo"})
        source4 = Source("plika", values={"qwe": "moha"}, parent="pl")
        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)
        self.source_manager.add_source(source3)
        self.source_manager.add_source(source4)

        try:
            self.source_manager.get_source("sse")
            self.fail("Expected LookupError")
        except LookupError as e:
            assert(e.__str__() == "Could not find source with name: sse")
    
    @pytest.mark.dbtest
    def test_get_sources_empty(self):
        sources = self.source_manager.get_sources()
        assert(len(sources) == 0)

    @pytest.mark.dbtest
    def test_add_with_wigos(self):
        source1 = Source("se", values={"bar": "baz"})
        source2 = Source("sekrn", values={"NOD":"sekrn","WIGOS": "0-20000-0-2032"}, parent="se")

        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)

        assert(self.source_manager.get_source("sekrn") == source2)
        source2 = Source("sekir", values={"qwe": "asd"}, parent="se")
        source3 = Source("pl", values={"bar": "boo"})
        source4 = Source("plika", values={"qwe": "moha"}, parent="pl")
        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)
        self.source_manager.add_source(source3)
        self.source_manager.add_source(source4)

        try:
            self.source_manager.get_source("sse")
            self.fail("Expected LookupError")
        except LookupError as e:
            assert(e.__str__() == "Could not find source with name: sse")
    
    @pytest.mark.dbtest
    def test_get_sources_empty(self):
        sources = self.source_manager.get_sources()
        assert(len(sources) == 0)

    @pytest.mark.dbtest
    def test_add_with_wigos(self):
        source1 = Source("se", values={"bar": "baz"})
        source2 = Source("sekrn", values={"NOD":"sekrn","WIGOS": "0-20000-0-2032"}, parent="se")

        self.source_manager.add_source(source1)
        self.source_manager.add_source(source2)

        assert(self.source_manager.get_source("sekrn") == source2)
