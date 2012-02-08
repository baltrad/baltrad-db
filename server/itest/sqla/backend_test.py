import datetime
import os
from tempfile import NamedTemporaryFile
import uuid

from nose.tools import eq_, ok_, raises
from nose.plugins.attrib import attr
from nose.plugins.skip import SkipTest

from baltrad.bdbserver.sqla import schema, storage
from baltrad.bdbserver.sqla.backend import (
    insert_metadata,
    insert_file,
    get_source_id,
    get_source_by_id,
    SqlAlchemyBackend
)

from baltrad.bdbserver.backend import (
    AttributeQuery,
    DuplicateEntry,
    FileQuery,
    IntegrityError,
)

from baltrad.bdbcommon.oh5.meta import Metadata, Source
from baltrad.bdbcommon.oh5.io import HlHdfMetadataWriter 
from baltrad.bdbcommon.oh5.node import Attribute, Group
from baltrad.bdbcommon import expr

backend = None

def setup_module():
    global backend
    url = os.environ.get("BDB_TEST_DB", "sqlite:///:memory:")
    try:
        backend = SqlAlchemyBackend(
            url,
            storage=storage.DatabaseStorage()
        )
    except:
        raise SkipTest("could not create backend (%s)" % url)
    backend.drop()
    backend.create()

def teardown_module():
    global backend
    if backend:
        backend.drop()
    

def create_metadata(what_object, what_date, what_time, what_source):
    meta = Metadata()
    meta.add_node("/", Group("what"))
    meta.add_node("/what", Attribute("object", what_object))
    meta.add_node("/what", Attribute("date", what_date))
    meta.add_node("/what", Attribute("time", what_time))
    meta.add_node("/what", Attribute("source", what_source))
    return meta

def write_metadata(meta):
    h5file = NamedTemporaryFile()
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
        global backend
        cls.backend = backend
        for src in cls.sources:
            cls.source_ids.append(cls.backend.add_source(src))
    
    @classmethod
    def teardown_class(cls):
        with cls.backend.get_connection() as conn:
            conn.execute(schema.sources.delete())

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
    def test_get_file(self):
        meta = create_metadata("pvol", "20000131", "131415", "NOD:eesur")
        h5file = write_metadata(meta)

        meta = self.backend.store_file(h5file.name)
        expected = open(h5file.name, "r").read()
        eq_(expected, self.backend.get_file(meta.bdb_uuid),
            "file content mismatch")
    
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
    
class TestSourceManagement(object):
    @classmethod
    def setup_class(cls):
        global backend
        cls.backend = backend
    
    def tearDown(self):
        with self.backend.get_connection() as conn:
            conn.execute(schema.files.delete())
            conn.execute(schema.sources.delete())
    
    @attr("dbtest")
    def test_add_source(self):
        source = Source("foo", values={"bar": "baz"})
        self.backend.add_source(source)

        sources = self.backend.get_sources()
        eq_(1, len(sources))
        eq_(source, sources[0])
    
    @attr("dbtest")
    @raises(DuplicateEntry)
    def test_add_source_duplicate(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("foo", values={"qwe": "asd"})

        self.backend.add_source(source1)
        self.backend.add_source(source2)

    @attr("dbtest")
    def test_update_source(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("qwe", values={"asd": "qaz"})
        self.backend.add_source(source1)

        self.backend.update_source("foo", source2)

        sources = self.backend.get_sources()
        eq_(1, len(sources))
        eq_(source2, sources[0])
    
    @attr("dbtest")
    @raises(LookupError)
    def test_update_source_not_found(self):
        source1 = Source("foo", values={"bar": "baz"})
        self.backend.update_source("qwe", source1)
    
    @attr("dbtest")
    @raises(DuplicateEntry)
    def test_update_source_name_conflict(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("qwe", values={"asd": "qaz"})
        self.backend.add_source(source1)
        self.backend.add_source(source2)

        self.backend.update_source("foo", source2)

    @attr("dbtest")
    def test_remove_source(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("qwe", values={"asd": "qaz"})
        self.backend.add_source(source1)
        self.backend.add_source(source2)
        
        eq_(True, self.backend.remove_source("foo"))

        sources = self.backend.get_sources()
        eq_(1, len(sources))
        eq_(source2, sources[0])
    
    @attr("dbtest")
    def test_remove_source_not_found(self):
        eq_(False, self.backend.remove_source("foo"))

    @attr("dbtest")
    @raises(IntegrityError)
    def test_remove_source_files_attached(self):
        source1 = Source("foo", values={"bar": "baz"})
        self.backend.add_source(source1)

        meta = create_metadata("pvol", "20000131", "131415", "bar:baz")
        h5file = write_metadata(meta)

        self.backend.store_file(h5file.name)

        self.backend.remove_source("foo")

    @attr("dbtest")
    def test_get_sources(self):
        source1 = Source("foo", values={"bar": "baz"})
        source2 = Source("bar", values={"qwe": "asd"})
        self.backend.add_source(source1)
        self.backend.add_source(source2)

        sources = self.backend.get_sources()
        eq_(2, len(sources))
        # in alphabetical order by name
        eq_(source2, sources[0])
        eq_(source1, sources[1])
    
    @attr("dbtest")
    def test_get_sources_empty(self):
        sources = self.backend.get_sources()
        eq_(0, len(sources))

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
    "bdb_file_size": 2000,
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
    "bdb_file_size": 3000,
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
    "bdb_file_size": 4000,
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
    "bdb_file_size": 5000,
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

def _insert_test_file(backend, file_):
    meta = Metadata()
    for k, v in file_.iteritems():
        if hasattr(meta, k):
            setattr(meta, k, v)
        else:
            _add_attribute(meta, k, v)
    with backend.get_connection() as conn:
        source_id = get_source_id(conn, meta.source())
        file_id =  insert_file(conn, meta, source_id)
        insert_metadata(conn, meta, file_id)

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
        Source("eesur", values={"NOD": "eesur", "PLC": "Syrgavere"}),
        Source("eehar", values={"NOD": "eehar", "PLC": "Harku"})
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
        with cls.backend.get_connection() as conn:
            conn.execute(schema.files.delete())
            conn.execute(schema.sources.delete())
    
    def setup(self):
        self.query = FileQuery()
    
    @attr("dbtest")
    def test_all_files(self):
        result = backend.execute_file_query(self.query)
        eq_(5, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)

    @attr("dbtest")
    def test_filter_by_uuid(self):
        uuid = "00000000-0000-0000-0004-000000000004"
        self.query.filter = expr.eq(
            expr.attribute("_bdb/uuid", "string"),
            expr.literal(uuid)
        )

        result = backend.execute_file_query(self.query)
        eq_(1, len(result))
        ok_({"uuid": uuid} in result)
    
    @attr("dbtest")
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
    
    @attr("dbtest")
    def test_filter_by_xsize(self):
        self.query.filter = expr.eq(
            expr.attribute("where/xsize", "long"),
            expr.literal(2)
        )

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_file_by_xsize_or_ysize(self):
        self.query.filter = expr.or_(
            expr.eq(
                expr.attribute("where/xsize", "long"),
                expr.literal(1)
            ),
            expr.eq(
                expr.attribute("where/ysize", "long"),
                expr.literal(2)
            )
        )

        result = backend.execute_file_query(self.query)
        eq_(3, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_filter_by_nominal_datetime(self):
        self.query.filter = expr.between(
            expr.add(
                expr.attribute("what/date", "date"),
                expr.attribute("what/time", "time")
            ),
            expr.literal(datetime.datetime(2000, 1, 1, 12, 1)),
            expr.literal(datetime.datetime(2001, 1, 1, 12, 1))
        )

        result = backend.execute_file_query(self.query)
        eq_(3, len(result)) 
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)

    @attr("dbtest")
    def test_filter_by_stored_datetime(self):
        self.query.filter = expr.between(
            expr.add(
                expr.attribute("_bdb/stored_date", "date"),
                expr.attribute("_bdb/stored_time", "time")
            ),
            expr.literal(datetime.datetime(2011, 1, 1, 12, 0, 2)),
            expr.literal(datetime.datetime(2011, 1, 1, 12, 0, 5))
        )

        result = backend.execute_file_query(self.query)
        eq_(3, len(result)) 
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)

    @attr("dbtest")
    def test_filter_by_stored_and_nominal_datetime_interval(self):
        # XXX the difference between the stored and nominal time in the
        # fixtures is a bit big for sanely testing this. They should be
        # moved closer together.
        stored_dt_attr = expr.add(
            expr.attribute("_bdb/stored_date", "date"),
            expr.attribute("_bdb/stored_time", "time")
        )
        nominal_dt_attr = expr.add(
            expr.attribute("what/date", "date"),
            expr.attribute("what/time", "time")
        )
        self.query.filter = expr.lt(
            expr.sub(stored_dt_attr, nominal_dt_attr),
            expr.literal(datetime.timedelta(days=4017, seconds=86342))
        )
        result = backend.execute_file_query(self.query)
        eq_(3, len(result)) 
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_filter_by_file_size(self):
        self.query.filter = expr.le(
            expr.attribute("_bdb/file_size", "long"),
            expr.literal(3000)
        )

        result = backend.execute_file_query(self.query)
        eq_(3, len(result)) 
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)

 
    @attr("dbtest")
    def test_filter_by_source_PLC(self):
        self.query.filter = expr.eq(
            expr.attribute("_bdb/source:PLC", "string"),
            expr.literal("Harku")
        )

        result = backend.execute_file_query(self.query)
        eq_(2, len(result)) 
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_filter_by_source_name_or_source_NOD(self):
        self.query.filter = expr.or_(
            expr.eq(
                expr.attribute("_bdb/source_name", "string"),
                expr.literal("eehar")
            ),
            expr.eq(
                expr.attribute("_bdb/source:NOD", "string"),
                expr.literal("eehar")
            )
        )

        result = backend.execute_file_query(self.query)
        eq_(2, len(result)) 
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_filter_by_source_name_like(self):
        self.query.filter = expr.like(
            expr.attribute("_bdb/source_name", "string"),
            "ee*"
        )

        result = backend.execute_file_query(self.query)
        eq_(5, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_filter_by_object_in(self):
        self.query.filter = expr.in_(
            expr.attribute("what/object", "string"),
            expr.literal(["CVOL", "SCAN"])
        )

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_filter_by_ysize_not_in(self):
        self.query.filter = expr.not_(
            expr.in_(
                expr.attribute("where/ysize", "long"),
                expr.literal([2, 4])
            )
        )

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_order_by(self):
        self.query.order = [
            expr.desc(expr.attribute("where/xsize", "long"))
        ]

        result = backend.execute_file_query(self.query)
        eq_(5, len(result))
        eq_(self.files[3]["bdb_uuid"], result[0]["uuid"])
        eq_(self.files[4]["bdb_uuid"], result[1]["uuid"])
        eq_(self.files[2]["bdb_uuid"], result[2]["uuid"])
        eq_(self.files[1]["bdb_uuid"], result[3]["uuid"])
        eq_(self.files[0]["bdb_uuid"], result[4]["uuid"])
    
    @attr("dbtest")
    def test_limit(self):
        self.query.limit = 2

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_skip(self):
        self.query.skip = 3

        result = backend.execute_file_query(self.query)
        eq_(2, len(result))
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @attr("dbtest")
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
        Source("eesur", {"NOD": "eesur", "PLC": "Syrgavere"}),
        Source("eehar", {"NOD": "eehar", "PLC": "Harku"})
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
        with cls.backend.get_connection() as conn:
            conn.execute(schema.files.delete())
            conn.execute(schema.sources.delete())
    
    def setup(self):
        self.query = AttributeQuery()
    
    @attr("dbtest")
    def test_fetch_uuid(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        
        result = self.query.execute(self.backend)
        eq_(5, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_fetch_uuid_filter_by_object(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
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
        
    @attr("dbtest")
    def test_fetch_xsize_filter_by_xsize(self):
        self.query.fetch = {
            "xsize": expr.attribute("where/xsize", "long")
        }
        self.query.filter = expr.eq(
            expr.attribute("where/xsize", "long"),
            expr.literal(2)
        )

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        eq_({"xsize": 2}, result[0])
        eq_({"xsize": 2}, result[1])
    
    @attr("dbtest")
    def test_fetch_uuid_filter_by_xsize_or_ysize(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.or_(
            expr.eq(
                expr.attribute("where/xsize", "long"),
                expr.literal(1)
            ),
            expr.eq(
                expr.attribute("where/ysize", "long"),
                expr.literal(2)
            )
        )

        result = self.query.execute(self.backend)
        eq_(4, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        eq_(2, len(filter(lambda r: r == {"uuid": self.files[4]["bdb_uuid"]}, result)))
    
    @attr("dbtest")
    def test_fetch_distinct_uuid_filter_by_xsize(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.distinct = True
        self.query.filter = expr.eq(
            expr.attribute("where/xsize", "long"),
            expr.literal(3)
        )

        result = self.query.execute(self.backend)
        eq_(1, len(result))
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)

    @attr("dbtest")
    def test_fetch_uuid_filter_by_nominal_datetime(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.between(
            expr.add(
                expr.attribute("what/date", "date"),
                expr.attribute("what/time", "time")
            ),
            expr.literal(datetime.datetime(2000, 1, 1, 12, 1)),
            expr.literal(datetime.datetime(2001, 1, 1, 12, 1))
        )

        result = self.query.execute(self.backend)
        eq_(3, len(result))
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[2]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)

    @attr("dbtest")
    def test_fetch_uuid_filter_by_source_PLC(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.eq(
            expr.attribute("_bdb/source:PLC", "string"),
            expr.literal("Harku")
        )

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_fetch_uuid_filter_by_source_name_or_source_name(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.or_(
            expr.eq(
                expr.attribute("_bdb/source_name", "string"),
                expr.literal("eehar")
            ),
            expr.eq(
                expr.attribute("_bdb/source_name", "string"),
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
    
    @attr("dbtest")
    def test_fetch_uuid_filter_by_source_name_and_source_name(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.and_(
            expr.eq(
                expr.attribute("_bdb/source_name", "string"),
                expr.literal("eehar")
            ),
            expr.eq(
                expr.attribute("_bdb/source_name", "string"),
                expr.literal("eesur")
            )
        )

        result = self.query.execute(self.backend)
        eq_(0, len(result))
    
    @attr("dbtest")
    def test_fetch_uuid_filter_by_source_name_like(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.like(
            expr.attribute("_bdb/source_name", "string"),
            "eeh*",
        )

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"uuid": self.files[1]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_order_by(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "long")
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
    
    @attr("dbtest")
    def test_limit(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
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
    
    @attr("dbtest")
    def test_fetch_max_xsize(self):
        self.query.fetch = {
            "max_xsize": expr.max(expr.attribute("where/xsize", "long"))
        }

        result = self.query.execute(self.backend)
        eq_(1, len(result))
        eq_({"max_xsize": 6}, result[0])
    
    @attr("dbtest")
    def test_fetch_count_ysize(self):
        self.query.fetch = {
            "count_ysize" : expr.count(expr.attribute("where/ysize", "long"))
        }

        result = self.query.execute(self.backend)
        eq_(1, len(result))
        eq_({"count_ysize": 6}, result[0])
    
    @attr("dbtest")
    def test_fetch_min_ysize_filter_by_what_object(self):
        self.query.fetch = {
            "min_ysize" : expr.min(expr.attribute("where/ysize", "long"))
        }
        self.query.filter = expr.eq(
            expr.attribute("what/object", "string"),
            expr.literal("CVOL")
        )

        result = self.query.execute(self.backend)
        eq_(1, len(result))
        eq_({"min_ysize": 4}, result[0])
    
    @attr("dbtest")
    def test_fetch_sum_xsize_group_by_source_name(self):
        self.query.fetch = {
            "source": expr.attribute("_bdb/source_name", "string"),
            "sum_xsize": expr.sum(expr.attribute("where/xsize", "long"))
        }
        self.query.group = [
            expr.attribute("_bdb/source_name", "string")
        ]

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"source": "eesur", "sum_xsize": 14} in result)
        ok_({"source": "eehar", "sum_xsize": 8} in result)
    
    @attr("dbtest")
    def test_fetch_sum_xsize_group_by_source_name_filter_by_date(self):
        self.query.fetch = {
            "source": expr.attribute("_bdb/source_name", "string"),
            "sum_xsize": expr.sum(expr.attribute("where/xsize", "long"))
        }
        self.query.group = [
            expr.attribute("_bdb/source_name", "string")
        ]
        self.query.filter = expr.eq(
            expr.attribute("what/date", "date"),
            expr.literal(datetime.date(2000, 1, 1))
        )

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"source": "eesur", "sum_xsize": 7} in result)
        ok_({"source": "eehar", "sum_xsize": 2} in result)
    
    @attr("dbtest")
    def test_fetch_max_xsize_group_by_source_name(self):
        self.query.fetch = {
            "source": expr.attribute("_bdb/source_name", "string"),
            "max_xsize": expr.max(expr.attribute("where/xsize", "long"))
        }
        self.query.group = [
            expr.attribute("_bdb/source_name", "string")
        ]

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"source": "eesur", "max_xsize": 5} in result)
        ok_({"source": "eehar", "max_xsize": 6} in result)
    
    @attr("dbtest")
    def test_fetch_max_xsize_min_ysize_group_by_source_name(self):
        self.query.fetch = {
            "source": expr.attribute("_bdb/source_name", "string"),
            "max_xsize": expr.max(expr.attribute("where/xsize", "long")),
            "min_ysize": expr.min(expr.attribute("where/ysize", "long"))
        }
        self.query.group = [
            expr.attribute("_bdb/source_name", "string")
        ]

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"source": "eesur", "max_xsize": 5, "min_ysize": 2} in result)
        ok_({"source": "eehar", "max_xsize": 6, "min_ysize": 2} in result)
    
    @attr("dbtest")
    def test_fetch_missing_value_group_by_source_name(self):
        self.query.fetch = {
            "source": expr.attribute("_bdb/source_name", "string"),
            "min_elangle": expr.min(expr.attribute("where/elangle", "double")),
        }
        self.query.group = [
            expr.attribute("_bdb/source_name", "string")
        ]

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"source": "eesur", "min_elangle": None} in result)
        ok_({"source": "eehar", "min_elangle": None} in result)
    
    @attr("dbtest")
    def test_fetch_uuid_filter_by_what_object_in(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.in_(
            expr.attribute("what/object", "string"),
            expr.literal(["CVOL", "SCAN"])
        )

        result = self.query.execute(self.backend)
        eq_(2, len(result))
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @attr("dbtest")
    def test_fetch_uuid_filter_by_xsize_not_in(self):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.not_(
            expr.in_(
                expr.attribute("where/xsize", "long"),
                expr.literal([2, 3])
            )
        )

        result = self.query.execute(self.backend)
        eq_(3, len(result))
        ok_({"uuid": self.files[0]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[3]["bdb_uuid"]} in result)
        ok_({"uuid": self.files[4]["bdb_uuid"]} in result)

