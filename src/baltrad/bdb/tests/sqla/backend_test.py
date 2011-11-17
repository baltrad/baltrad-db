import datetime
import os
from tempfile import NamedTemporaryFile
import uuid

from nose.tools import eq_, ok_, raises

from baltrad.bdb.sqla import schema
from baltrad.bdb.sqla.backend import (
    _insert_metadata,
    _insert_file,
    SqlAlchemyBackend
)

from baltrad.bdb.backend import (
    AttributeQuery,
    DuplicateEntry,
    FileQuery
)

from baltrad.bdb.oh5.meta import Metadata, Source
from baltrad.bdb.oh5.io import HlHdfMetadataWriter 
from baltrad.bdb.oh5.node import Attribute, Group
from baltrad.bdb import expr

backend = None

def setup_module():
    global backend
    url = os.environ.get("BDB_TEST_DB", "sqlite:///:memory:")
    backend = SqlAlchemyBackend(url)
    schema.meta.drop_all(backend.get_connection())
    schema.meta.create_all(backend.get_connection())

def teardown_module():
    global backend
    schema.meta.drop_all(backend.get_connection())
    
class TestSqlAlchemyBackendItest(object):
    backend = None

    sources = [
        Source({"_name": "eesur", "NOD": "eesur", "PLC": "Syrgavere"}),
        Source({"_name": "eehar", "NOD": "eehar", "PLC": "Harku"})
    ]

    source_ids = []

    @classmethod
    def setup_class(cls):
        global backend
        cls.backend = backend
        for src in cls.sources:
            cls.source_ids.append(cls.backend.add_source(src))
    
    @classmethod
    def teardown_class(cls):
        cls.backend.get_connection().execute(schema.sources.delete())

    def tearDown(self):
        self.backend.get_connection().execute(schema.files.delete())

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
        source = self.backend.get_source_by_id(self.source_ids[0])
        eq_(3, len(source))
        eq_("eesur", source["NOD"])
        eq_("eesur", source.name)
        eq_("Syrgavere", source["PLC"])

    def test_get_source_id(self):
        source = {"NOD": "eesur"}
        eq_(self.source_ids[0], self.backend.get_source_id(source))
    
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

###
# src name obj  date       time  xsize ysize
# sur td1  PVOL 2000-01-01 12:00     1     2
# har td2  PVOL 2000-01-01 12:01     2     2
# sur td3  PVOL 2000-01-01 12:02     3
#                                    3
# har td4  CVOL 2001-01-01 12:00     6     4
#                                          5
# sur td5  SCAN 2002-02-01 12:00     5     2
#                                    2     5
FILES = [{
    "what_object": "PVOL",
    "what_date": datetime.date(2000, 1, 1),
    "what_time": datetime.time(12, 0),
    "what_source": "NOD:eesur",
    "bdb_uuid": "00000000-0000-0000-0004-000000000001",
    "bdb_metadata_hash": "hash1",
    "bdb_stored_date": datetime.date(2011, 1, 1),
    "bdb_stored_time": datetime.time(12, 0, 1),
    "bdb_file_size": 1000,
    "/dataset1/where/xsize": 1,
    "/dataset1/where/ysize": 2,
}, {
    "what_object": "PVOL",
    "what_date": datetime.date(2000, 1, 1),
    "what_time": datetime.time(12, 1),
    "what_source": "NOD:eehar",
    "bdb_uuid": "00000000-0000-0000-0004-000000000002",
    "bdb_metadata_hash": "hash2",
    "bdb_stored_date": datetime.date(2011, 1, 1),
    "bdb_stored_time": datetime.time(12, 0, 2),
    "bdb_file_size": 1000,
    "/dataset1/where/xsize": 2,
    "/dataset1/where/ysize": 2,
}, {
    "what_object": "PVOL",
    "what_date": datetime.date(2000, 1, 1),
    "what_time": datetime.time(12, 2),
    "what_source": "NOD:eesur",
    "bdb_uuid": "00000000-0000-0000-0004-000000000003",
    "bdb_metadata_hash": "hash3",
    "bdb_stored_date": datetime.date(2011, 1, 1),
    "bdb_stored_time": datetime.time(12, 0, 3),
    "bdb_file_size": 1000,
    "/dataset1/where/xsize": 3,
    "/dataset2/where/xsize": 3,    
}, {
    "what_object": "CVOL",
    "what_date": datetime.date(2001, 1, 1),
    "what_time": datetime.time(12, 0),
    "what_source": "NOD:eehar",
    "bdb_uuid": "00000000-0000-0000-0004-000000000004",
    "bdb_metadata_hash": "hash4",
    "bdb_stored_date": datetime.date(2011, 1, 1),
    "bdb_stored_time": datetime.time(12, 0, 4),
    "bdb_file_size": 1000,
    "/dataset1/where/xsize": 6,
    "/dataset1/where/ysize": 4,
    "/dataset2/where/ysize": 5,
}, {
    "what_object": "SCAN",
    "what_date": datetime.date(2002, 2, 1),
    "what_time": datetime.time(12, 0),
    "what_source": "NOD:eesur",
    "bdb_uuid": "00000000-0000-0000-0004-000000000005",
    "bdb_metadata_hash": "hash5",
    "bdb_stored_date": datetime.date(2011, 1, 1),
    "bdb_stored_time": datetime.time(12, 0, 5),
    "bdb_file_size": 1000,
    "/dataset1/where/xsize": 5,
    "/dataset1/where/ysize": 2,
    "/dataset2/where/xsize": 2,
    "/dataset2/where/ysize": 5,
}]

def _insert_test_files(backend):
    global FILES
    for file_ in FILES:
        _insert_test_file(backend, file_)
    return FILES

def _insert_test_file(conn, file_):
    meta = Metadata()
    for k, v in file_.iteritems():
        if hasattr(meta, k):
            setattr(meta, k, v)
        else:
            _add_attribute(meta, k, v)
    source_id = backend.get_source_id(meta.source())
    conn = backend.get_connection()
    file_id =  _insert_file(conn, meta, source_id)
    _insert_metadata(conn, meta, file_id)

def _add_attribute(meta, path, value):
    path = path.split("/")
    path.pop(0)
    node = meta.root()
    for name in path[:-1]:
        if not node.has_child_by_name(name):
            node = node.add_child(Group(name))
        else:
            node = node.child_by_name(name)
    
    node.add_child(Attribute(path[-1], value=value))

class TestFileQuery(object):
    backend = None

    sources = [
        Source({"_name": "eesur", "NOD": "eesur", "PLC": "Syrgavere"}),
        Source({"_name": "eehar", "NOD": "eehar", "PLC": "Harku"})
    ]

    @classmethod
    def setup_class(cls):
        global backend
        cls.backend = backend
        for src in cls.sources:
            cls.backend.add_source(src)
        cls.files = _insert_test_files(cls.backend)
        
    @classmethod
    def teardown_class(cls):
        cls.backend.get_connection().execute(schema.files.delete())
        cls.backend.get_connection().execute(schema.sources.delete())
    
    def setup(self):
        self.query = FileQuery()
    
    def test_all_files(self):
        result = backend.execute_file_query(self.query)
        eq_(5, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)

    def test_filter_by_uuid(self):
        uuid = "00000000-0000-0000-0004-000000000004"
        self.query.filter = expr.eq(
            expr.attribute("file:uuid", "string"),
            expr.literal(uuid)
        )

        result = backend.execute_file_query(self.query)
        eq_(1, len(result))
        ok_({"uuid": uuid} in result)
    
    def test_filter_by_object(self):
        self.query.filter = expr.eq(
            expr.attribute("what/object", "string"),
            expr.literal("PVOL")
        )

        result = backend.execute_file_query(self.query)
        eq_(3, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
    
    def test_filter_by_xsize(self):
        self.query.filter = expr.eq(
            expr.attribute("where/xsize", "int"),
            expr.literal(2)
        )

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    def test_file_by_xsize_or_ysize(self):
        self.query.filter = expr.or_(
            expr.eq(
                expr.attribute("where/xsize", "int"),
                expr.literal(1)
            ),
            expr.eq(
                expr.attribute("where/ysize", "int"),
                expr.literal(2)
            )
        )

        result = backend.execute_file_query(self.query)
        eq_(3, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    def test_filter_by_combined_datetime(self):
        self.query.filter = expr.between(
            expr.add(
                expr.attribute("what/date", "date"),
                expr.attribute("what/time", "time")
            ),
            expr.literal(datetime.datetime(2000, 1, 1, 12, 1)),
            expr.literal(datetime.datetime(2001, 1, 1, 12, 0))
        )

        result = backend.execute_file_query(self.query)
        eq_(3, len(result)) 
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    def test_filter_by_source_PLC(self):
        self.query.filter = expr.eq(
            expr.attribute("what/source:PLC", "string"),
            expr.literal("Harku")
        )

        result = backend.execute_file_query(self.query)
        eq_(2, len(result)) 
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    def test_filter_by_source_name_or_source_NOD(self):
        self.query.filter = expr.or_(
            expr.eq(
                expr.attribute("what/source:_name", "string"),
                expr.literal("eehar")
            ),
            expr.eq(
                expr.attribute("what/source:NOD", "string"),
                expr.literal("eehar")
            )
        )

        result = backend.execute_file_query(self.query)
        eq_(2, len(result)) 
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    def test_filter_by_source_name_like(self):
        self.query.filter = expr.like(
            expr.attribute("what/source:_name", "string"),
            "ee*"
        )

        result = backend.execute_file_query(self.query)
        eq_(5, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    def test_filter_by_object_in(self):
        self.query.filter = expr.in_(
            expr.attribute("what/object", "string"),
            expr.literal(["CVOL", "SCAN"])
        )

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    def test_file_by_ysize_not_in(self):
        self.query.filter = expr.not_(
            expr.in_(
                expr.attribute("where/ysize", "int"),
                expr.literal([2, 4])
            )
        )

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    def test_order_by(self):
        self.query.order = [
            expr.desc(expr.attribute("where/xsize", "int"))
        ]

        result = backend.execute_file_query(self.query)
        eq_(5, len(result))
        eq_(self.files[3]["bdb_uuid"], result[0]["uuid"])
        eq_(self.files[4]["bdb_uuid"], result[1]["uuid"])
        eq_(self.files[2]["bdb_uuid"], result[2]["uuid"])
        eq_(self.files[1]["bdb_uuid"], result[3]["uuid"])
        eq_(self.files[0]["bdb_uuid"], result[4]["uuid"])
    
    def test_limit(self):
        self.query.limit = 2

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
    
    def test_skip(self):
        self.query.skip = 3

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    def test_skip_with_limit(self):
        self.query.skip = 2
        self.query.limit = 2

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)


class TestAttributeQuery(object):
    backend = None

    sources = [
        Source({"_name": "eesur", "NOD": "eesur", "PLC": "Syrgavere"}),
        Source({"_name": "eehar", "NOD": "eehar", "PLC": "Harku"})
    ]

    @classmethod
    def setup_class(cls):
        global backend
        cls.backend = backend
        for src in cls.sources:
            cls.backend.add_source(src)
        cls.files = _insert_test_files(cls.backend)
        
    @classmethod
    def teardown_class(cls):
        cls.backend.get_connection().execute(schema.files.delete())
        cls.backend.get_connection().execute(schema.sources.delete())
    
    def setup(self):
        self.query = AttributeQuery()
    
    def test_fetch_uuid(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        
        result = self.query.execute(self.backend)
        eq_(5, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    def test_fetch_uuid_filter_by_object(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.filter = expr.eq(
            expr.attribute("what/object", "string"),
            expr.literal("PVOL")
        )

        result = self.query.execute(self.backend)
        eq_(3, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        
    def test_fetch_xsize_filter_by_xsize(self):
        self.query.fetch = {
            "xsize": expr.attribute("where/xsize", "int")
        }
        self.query.filter = expr.eq(
            expr.attribute("where/xsize", "int"),
            expr.literal(2)
        )

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        eq_({"xsize": 2}, result[0])
        eq_({"xsize": 2}, result[1])
    
    def test_fetch_uuid_filter_by_xsize_or_ysize(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.filter = expr.or_(
            expr.eq(
                expr.attribute("where/xsize", "int"),
                expr.literal(1)
            ),
            expr.eq(
                expr.attribute("where/ysize", "int"),
                expr.literal(2)
            )
        )

        result = self.query.execute(self.backend)
        eq_(4, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        eq_(2, len(filter(lambda r: r == {"uuid": self.files[4]["bdb_uuid"]}, result)))
    
    def test_fetch_distinct_uuid_filter_by_xsize(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.distinct = True
        self.query.filter = expr.eq(
            expr.attribute("where/xsize", "int"),
            expr.literal(3)
        )

        result = self.query.execute(self.backend)
        eq_(1, len(result))
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)

    def test_fetch_uuid_filter_by_combined_stored_datetime(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.filter = expr.between(
            expr.add(
                expr.attribute("what/date", "date"),
                expr.attribute("what/time", "time")
            ),
            expr.literal(datetime.datetime(2000, 1, 1, 12, 1)),
            expr.literal(datetime.datetime(2001, 1, 1, 12, 0))
        )

        result = self.query.execute(self.backend)
        eq_(3, len(result))
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)

    def test_fetch_uuid_filter_by_source_PLC(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.filter = expr.eq(
            expr.attribute("what/source:PLC", "string"),
            expr.literal("Harku")
        )

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    def test_fetch_uuid_filter_by_source_name_or_source_name(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.filter = expr.or_(
            expr.eq(
                expr.attribute("what/source:_name", "string"),
                expr.literal("eehar")
            ),
            expr.eq(
                expr.attribute("what/source:_name", "string"),
                expr.literal("eesur")
            )
        )

        result = self.query.execute(self.backend)
        eq_(5, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    def test_fetch_uuid_filter_by_source_name_and_source_name(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.filter = expr.and_(
            expr.eq(
                expr.attribute("what/source:_name", "string"),
                expr.literal("eehar")
            ),
            expr.eq(
                expr.attribute("what/source:_name", "string"),
                expr.literal("eesur")
            )
        )

        result = self.query.execute(self.backend)
        eq_(0, len(result))
    
    def test_fetch_uuid_filter_by_source_name_like(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.filter = expr.like(
            expr.attribute("what/source:_name", "string"),
            "eeh*",
        )

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    def test_order_by(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "int")
        }
        self.query.order = [
            expr.desc(
                expr.add(
                    expr.attribute("what/date", "date"),
                    expr.attribute("what/time", "time")
                )
            )
        ]

        result = self.query.execute(self.backend)
        eq_(5, len(result))
        eq_({"uuid": self.files[4]["bdb_uuid"]}, result[0])
        eq_({"uuid": self.files[3]["bdb_uuid"]}, result[1])
        eq_({"uuid": self.files[2]["bdb_uuid"]}, result[2])
        eq_({"uuid": self.files[1]["bdb_uuid"]}, result[3])
        eq_({"uuid": self.files[0]["bdb_uuid"]}, result[4])
    
    def test_limit(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.order = [
            expr.desc(
                expr.add(
                    expr.attribute("what/date", "date"),
                    expr.attribute("what/time", "time")
                )
            )
        ]
        self.query.limit = 2

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        eq_({"uuid": self.files[4]["bdb_uuid"]}, result[0])
        eq_({"uuid": self.files[3]["bdb_uuid"]}, result[1])
    
    def test_fetch_max_xsize(self):
        self.query.fetch = {
            "max_xsize": expr.max(expr.attribute("where/xsize", "int"))
        }

        result = self.query.execute(self.backend)
        eq_(1, len(result))
        eq_({"max_xsize": 6}, result[0])
    
    def test_fetch_count_ysize(self):
        self.query.fetch = {
            "count_ysize" : expr.count(expr.attribute("where/ysize", "int"))
        }

        result = self.query.execute(self.backend)
        eq_(1, len(result))
        eq_({"count_ysize": 6}, result[0])
    
    def test_fetch_min_ysize_filter_by_what_object(self):
        self.query.fetch = {
            "min_ysize" : expr.min(expr.attribute("where/ysize", "int"))
        }
        self.query.filter = expr.eq(
            expr.attribute("what/object", "string"),
            expr.literal("CVOL")
        )

        result = self.query.execute(self.backend)
        eq_(1, len(result))
        eq_({"min_ysize": 4}, result[0])
    
    def test_fetch_sum_xsize_group_by_source_name(self):
        self.query.fetch = {
            "source": expr.attribute("what/source:_name", "string"),
            "sum_xsize": expr.sum(expr.attribute("where/xsize", "int"))
        }
        self.query.group = [
            expr.attribute("what/source:_name", "string")
        ]

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"source": "eesur", "sum_xsize": 14} in result)
        ok_({"source": "eehar", "sum_xsize": 8} in result)
    
    def test_fetch_sum_xsize_group_by_source_name_filter_by_date(self):
        self.query.fetch = {
            "source": expr.attribute("what/source:_name", "string"),
            "sum_xsize": expr.sum(expr.attribute("where/xsize", "int"))
        }
        self.query.group = [
            expr.attribute("what/source:_name", "string")
        ]
        self.query.filter = expr.eq(
            expr.attribute("what/date", "date"),
            expr.literal(datetime.date(2000, 1, 1))
        )

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"source": "eesur", "sum_xsize": 7} in result)
        ok_({"source": "eehar", "sum_xsize": 2} in result)
    
    def test_fetch_max_xsize_group_by_source_name(self):
        self.query.fetch = {
            "source": expr.attribute("what/source:_name", "string"),
            "max_xsize": expr.max(expr.attribute("where/xsize", "int"))
        }
        self.query.group = [
            expr.attribute("what/source:_name", "string")
        ]

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"source": "eesur", "max_xsize": 5} in result)
        ok_({"source": "eehar", "max_xsize": 6} in result)
    
    def test_fetch_max_xsize_min_ysize_group_by_source_name(self):
        self.query.fetch = {
            "source": expr.attribute("what/source:_name", "string"),
            "max_xsize": expr.max(expr.attribute("where/xsize", "int")),
            "min_ysize": expr.min(expr.attribute("where/ysize", "int"))
        }
        self.query.group = [
            expr.attribute("what/source:_name", "string")
        ]

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"source": "eesur", "max_xsize": 5, "min_ysize": 2} in result)
        ok_({"source": "eehar", "max_xsize": 6, "min_ysize": 2} in result)
    
    def test_fetch_missing_value_group_by_source_name(self):
        self.query.fetch = {
            "source": expr.attribute("what/source:_name", "string"),
            "min_elangle": expr.min(expr.attribute("where/elangle", "double")),
        }
        self.query.group = [
            expr.attribute("what/source:_name", "string")
        ]

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"source": "eesur", "min_elangle": None} in result)
        ok_({"source": "eehar", "min_elangle": None} in result)
    
    def test_fetch_uuid_filter_by_what_object_in(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.filter = expr.in_(
            expr.attribute("what/object", "string"),
            expr.literal(["CVOL", "SCAN"])
        )

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    def test_fetch_uuid_filter_by_xsize_not_in(self):
        self.query.fetch = {
            "uuid": expr.attribute("file:uuid", "string")
        }
        self.query.filter = expr.not_(
            expr.in_(
                expr.attribute("where/xsize", "int"),
                expr.literal([2, 3])
            )
        )

        result = self.query.execute(self.backend)
        eq_(3, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)

