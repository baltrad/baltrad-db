import datetime
import pytest

from baltrad.bdbcommon import expr, oh5
from baltrad.bdbserver import backend

from baltrad.bdbserver.sqla import (
    backend as sqla_backend,
    schema
)

from .conftest import bdb_backend

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
    "what_source": "NOD:eesur,CMT:file1",
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
    "what_source": "NOD:eehar,CMT:file2",
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
    "what_source": "NOD:eesur,CMT:file3",
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
    "what_source": "NOD:eehar,CMT:file4",
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
    "what_source": "NOD:eesur,CMT:file5",
    "bdb_uuid": "00000000-0000-0000-0004-000000000005",
    "bdb_metadata_hash": "hash5",
    "bdb_stored_date": datetime.date(2011, 1, 1),
    "bdb_stored_time": datetime.time(12, 0, 5),
    "bdb_file_size": 5000,
    "/dataset1/where/xsize": 5,
    "/dataset1/where/ysize": 2,
    "/dataset2/where/xsize": 2,
    "/dataset2/where/ysize": 5,
    "/dataset1/data1/what/nodata":255.0,
    "/dataset1/data2/what/nodata":254.0,
    "/dataset2/data1/what/nodata":253.0,
    "/dataset2/data2/what/nodata":252.0,
    "/dataset2/data2/where/nodata":252.0,
}, {
    "what_object": "SCAN",
    "what_date": datetime.date(2021, 5, 26),
    "what_time": datetime.time(12, 0),
    "what_source": "PLC:Kiruna,NOD:sekrn",
    "bdb_uuid": "00000000-0000-0000-0004-00000000000a",
    "bdb_metadata_hash": "hash6",
    "bdb_stored_date": datetime.date(2021, 5, 26),
    "bdb_stored_time": datetime.time(12, 0, 5),
    "bdb_file_size": 6000,
    "/dataset1/where/xsize": 7,
    "/dataset1/where/ysize": 7,
    "/dataset2/where/xsize": 7,
    "/dataset2/where/ysize": 7,
    "/dataset1/data1/what/nodata":255.0,
    "/dataset1/data2/what/nodata":254.0,
    "/dataset2/data1/what/nodata":253.0,
    "/dataset2/data2/what/nodata":252.0,
    "/dataset2/data2/where/nodata":252.0,
}, {
    "what_object": "SCAN",
    "what_date": datetime.date(2021, 5, 26),
    "what_time": datetime.time(12, 1),
    "what_source": "WIGOS:0-20000-0-2600",
    "bdb_uuid": "00000000-0000-0000-0004-00000000000b",
    "bdb_metadata_hash": "hash6",
    "bdb_stored_date": datetime.date(2021, 5, 26),
    "bdb_stored_time": datetime.time(12, 0, 5),
    "bdb_file_size": 6000,
    "/dataset1/where/xsize": 8,
    "/dataset1/where/ysize": 8,
    "/dataset2/where/xsize": 8,
    "/dataset2/where/ysize": 8,
    "/dataset1/data1/what/nodata":255.0,
    "/dataset1/data2/what/nodata":254.0,
    "/dataset2/data1/what/nodata":253.0,
    "/dataset2/data2/what/nodata":252.0,
    "/dataset2/data2/where/nodata":252.0,
}
]

def _insert_test_files(backend):
    global FILES
    for file_ in FILES:
        _insert_test_file(backend, file_)
    return FILES

def _insert_test_file(backend, file_):
    meta = oh5.Metadata()
    for k, v in file_.items():
        if hasattr(meta, k):
            setattr(meta, k, v)
        else:
            _add_attribute(meta, k, v)

    with backend.get_connection() as conn:
        source_id = sqla_backend.get_source_id(conn, meta.source())
        file_id =  sqla_backend.insert_file(conn, meta, source_id)
        sqla_backend.associate_what_source(conn, file_id, meta.source())
        sqla_backend.insert_metadata(conn, meta, file_id)
        conn.commit()

def _add_attribute(meta, path, value):
    path = path.split("/")
    path.pop(0)
    node = meta.root()
    for name in path[:-1]:
        if not node.has_child_by_name(name):
            node = node.add_child(oh5.Group(name))
        else:
            node = node.child_by_name(name)
    
    node.add_child(oh5.Attribute(path[-1], value=value))

@pytest.mark.dbtest
class TestFileQuery(object):
    backend = None

    sources = [
        oh5.Source("eesur", values={"NOD": "eesur", "PLC": "Syrgavere"}),
        oh5.Source("eehar", values={"NOD": "eehar", "PLC": "Harku"}),
        oh5.Source("sekrn", values={"NOD": "sekrn", "WIGOS": "0-20000-0-2032", "PLC": "Kiruna"}),
        oh5.Source("sevax", values={"NOD": "sevax", "WMO": "02600", "WIGOS": "0-20000-0-2600", "PLC": "Vara"})
    ]

    @pytest.fixture(autouse=True)
    def setup_class_fixture(self, bdb_backend):
        if not bdb_backend:
            pytest.skip("no backend defined")

        for src in self.sources:
            bdb_backend.get_source_manager().add_source(src)
        self.files = _insert_test_files(bdb_backend)
        self.query = backend.FileQuery()

        yield

        with bdb_backend.get_connection() as conn:
            conn.execute(schema.files.delete())
            conn.execute(schema.sources.delete())
            conn.commit()

    @pytest.mark.dbtest
    def test_all_files(self, bdb_backend):
        result = bdb_backend.execute_file_query(self.query)
        assert(7 == len(result))
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
        assert({"uuid": self.files[5]["bdb_uuid"]} in result)
        assert({"uuid": self.files[6]["bdb_uuid"]} in result)

    @pytest.mark.dbtest
    def test_filter_by_uuid(self, bdb_backend):
        uuid = "00000000-0000-0000-0004-000000000004"
        self.query.filter = expr.eq(
            expr.attribute("_bdb/uuid", "string"),
            expr.literal(uuid)
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(1 == len(result))
        assert({"uuid": uuid} in result)
    
    @pytest.mark.dbtest
    def test_filter_by_object(self, bdb_backend):
        self.query.filter = expr.eq(
            expr.attribute("what/object", "string"),
            expr.literal("PVOL")
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(3 == len(result))
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_filter_by_xsize(self, bdb_backend):
        self.query.filter = expr.eq(
            expr.attribute("where/xsize", "long"),
            expr.literal(2)
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(2 == len(result))
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_file_by_xsize_or_ysize(self, bdb_backend):
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

        result = bdb_backend.execute_file_query(self.query)
        assert(3 == len(result))
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_filter_by_nominal_datetime(self, bdb_backend):
        self.query.filter = expr.between(
            expr.add(
                expr.attribute("what/date", "date"),
                expr.attribute("what/time", "time")
            ),
            expr.literal(datetime.datetime(2000, 1, 1, 12, 1)),
            expr.literal(datetime.datetime(2001, 1, 1, 12, 1))
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(3 == len(result)) 
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)

    @pytest.mark.dbtest
    def test_filter_by_stored_datetime(self, bdb_backend):
        self.query.filter = expr.between(
            expr.add(
                expr.attribute("_bdb/stored_date", "date"),
                expr.attribute("_bdb/stored_time", "time")
            ),
            expr.literal(datetime.datetime(2011, 1, 1, 12, 0, 2)),
            expr.literal(datetime.datetime(2011, 1, 1, 12, 0, 5))
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(3 == len(result)) 
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)

    @pytest.mark.dbtest
    def test_filter_by_stored_and_nominal_datetime_interval(self, bdb_backend):
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
        result = bdb_backend.execute_file_query(self.query)
        assert(5 == len(result)) 
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
        assert({"uuid": self.files[5]["bdb_uuid"]} in result)
        assert({"uuid": self.files[6]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_filter_by_file_size(self, bdb_backend):
        self.query.filter = expr.le(
            expr.attribute("_bdb/file_size", "long"),
            expr.literal(3000)
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(3 == len(result)) 
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)

 
    @pytest.mark.dbtest
    def test_filter_by_bdb_source_PLC(self, bdb_backend):
        self.query.filter = expr.eq(
            expr.attribute("_bdb/source:PLC", "string"),
            expr.literal("Harku")
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(2==len(result)) 
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_filter_by_bdb_source_name_or_source_NOD(self, bdb_backend):
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

        result = bdb_backend.execute_file_query(self.query)
        assert(2==len(result)) 
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)

    @pytest.mark.dbtest
    def test_filter_by_WIGOS(self, bdb_backend):
        self.query.filter = expr.eq(
          expr.attribute("_bdb/source:WIGOS", "string"),
          expr.literal("0-20000-0-2032")
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(1==len(result)) 
        assert({"uuid": self.files[5]["bdb_uuid"]} in result)

    @pytest.mark.dbtest
    def test_filter_by_WIGOS_2(self, bdb_backend):
        self.query.filter = expr.eq(
          expr.attribute("_bdb/source:WIGOS", "string"),
          expr.literal("0-20000-0-2600")
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(1==len(result)) 
        assert({"uuid": self.files[6]["bdb_uuid"]} in result)


    @pytest.mark.dbtest
    def test_filter_by_bdb_source_name_like(self, bdb_backend):
        self.query.filter = expr.like(
            expr.attribute("_bdb/source_name", "string"),
            "ee*"
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(5==len(result))
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_filter_by_what_source_CMT(self, bdb_backend):
        self.query.filter = expr.eq(
            expr.attribute("what/source:CMT", "string"),
            expr.literal("file3")
        )
        result =  bdb_backend.execute_file_query(self.query)
        assert(1==len(result))
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)

    @pytest.mark.dbtest
    def test_filter_by_what_source_CMT_or_NOD(self, bdb_backend):
        self.query.filter = expr.or_(
            expr.eq(
                expr.attribute("what/source:CMT", "string"),
                expr.literal("file3")
            ),
            expr.eq(
                expr.attribute("what/source:NOD", "string"),
                expr.literal("eehar")
            )
        )
        result = bdb_backend.execute_file_query(self.query)
        assert(3==len(result))
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)

    @pytest.mark.dbtest
    def test_filter_by_object_in(self, bdb_backend):
        self.query.filter = expr.in_(
            expr.attribute("what/object", "string"),
            expr.literal(["CVOL", "SCAN"])
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(4==len(result))
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
        assert({"uuid": self.files[5]["bdb_uuid"]} in result)
        assert({"uuid": self.files[6]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_filter_by_ysize_not_in(self, bdb_backend):
        self.query.filter = expr.not_(
            expr.in_(
                expr.attribute("where/ysize", "long"),
                expr.literal([2, 4])
            )
        )

        result = bdb_backend.execute_file_query(self.query)
        assert(4==len(result))
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
        assert({"uuid": self.files[5]["bdb_uuid"]} in result)
        assert({"uuid": self.files[6]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_order_by(self, bdb_backend):
        self.query.order = [
            expr.desc(expr.attribute("where/xsize", "long"))
        ]

        result = bdb_backend.execute_file_query(self.query)
        assert(7==len(result))
        assert(self.files[6]["bdb_uuid"]==result[0]["uuid"])
        assert(self.files[5]["bdb_uuid"]==result[1]["uuid"])
        assert(self.files[3]["bdb_uuid"]==result[2]["uuid"])
        assert(self.files[4]["bdb_uuid"]==result[3]["uuid"])
        assert(self.files[2]["bdb_uuid"]==result[4]["uuid"])
        assert(self.files[1]["bdb_uuid"]==result[5]["uuid"])
        assert(self.files[0]["bdb_uuid"]==result[6]["uuid"])
    
    @pytest.mark.dbtest
    def test_limit(self, bdb_backend):
        self.query.limit = 2

        result = bdb_backend.execute_file_query(self.query)
        assert(2==len(result))
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_skip(self, bdb_backend):
        self.query.skip = 3

        result = bdb_backend.execute_file_query(self.query)
        assert(4==len(result))
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
        assert({"uuid": self.files[5]["bdb_uuid"]} in result)
        assert({"uuid": self.files[6]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_skip_with_limit(self, bdb_backend):
        self.query.skip = 2
        self.query.limit = 2

        result = bdb_backend.execute_file_query(self.query)
        assert(2==len(result))
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)

@pytest.mark.dbtest
class TestAttributeQuery(object):
    backend = None

    sources = [
        oh5.Source("eesur", {"NOD": "eesur", "PLC": "Syrgavere"}),
        oh5.Source("eehar", {"NOD": "eehar", "PLC": "Harku"}),
        oh5.Source("sekrn", values={"NOD": "sekrn", "WIGOS": "0-20000-0-2032", "PLC": "Kiruna"}),
        oh5.Source("sevax", values={"NOD": "sevax", "WMO": "02600", "WIGOS": "0-20000-0-2600", "PLC": "Vara"})
    ]

    @pytest.fixture(autouse=True)
    def setup_fixture(self, bdb_backend):
        if not bdb_backend:
            pytest.skip("no backend defined")

        with bdb_backend.get_connection() as conn:
            conn.execute(schema.files.delete())
            conn.execute(schema.sources.delete())
            conn.commit()

        for src in self.sources:
            bdb_backend.get_source_manager().add_source(src)

        self.files = _insert_test_files(bdb_backend)
        self.query = backend.AttributeQuery()

        yield

        with bdb_backend.get_connection() as conn:
            conn.execute(schema.files.delete())
            conn.execute(schema.sources.delete())
            conn.commit()

    # @classmethod
    # def setup_class(cls):
    #     cls.backend = get_backend()
    #     if not cls.backend:
    #         return

    #     for src in cls.sources:
    #         cls.backend.get_source_manager().add_source(src)
    #     cls.files = _insert_test_files(cls.backend)
        
    # @classmethod
    # def teardown_class(cls):
    #     if not cls.backend:
    #         return

    #     with cls.backend.get_connection() as conn:
    #         conn.execute(schema.files.delete())
    #         conn.execute(schema.sources.delete())
    
    # def setup(self):
    #     if not self.backend:
    #         pytest.skip("no backend defined")

    #     self.query = backend.AttributeQuery()
    
    @pytest.mark.dbtest
    def test_fetch_uuid(self, bdb_backend):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        
        result = self.query.execute(bdb_backend)
        assert(7==len(result))
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
        assert({"uuid": self.files[5]["bdb_uuid"]} in result)
        assert({"uuid": self.files[6]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_fetch_uuid_filter_by_object(self, bdb_backend):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.eq(
            expr.attribute("what/object", "string"),
            expr.literal("PVOL")
        )

        result = self.query.execute(bdb_backend)
        assert(3 == len(result))
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
        
    @pytest.mark.dbtest
    def test_fetch_xsize_filter_by_xsize(self, bdb_backend):
        self.query.fetch = {
            "xsize": expr.attribute("where/xsize", "long")
        }
        self.query.filter = expr.eq(
            expr.attribute("where/xsize", "long"),
            expr.literal(2)
        )

        result = self.query.execute(bdb_backend)
        assert(2 == len(result))
        assert({"xsize": 2} == result[0])
        assert({"xsize": 2} == result[1])
    
    @pytest.mark.dbtest
    def test_fetch_uuid_filter_by_xsize_or_ysize(self, bdb_backend):
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

        result = self.query.execute(bdb_backend)
        assert(4==len(result))
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert(2 == len(list(filter(lambda r: r == {"uuid": self.files[4]["bdb_uuid"]}, result))))
    
    @pytest.mark.dbtest
    def test_fetch_distinct_uuid_filter_by_xsize(self, bdb_backend):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.distinct = True
        self.query.filter = expr.eq(
            expr.attribute("where/xsize", "long"),
            expr.literal(3)
        )

        result = self.query.execute(bdb_backend)
        assert(1==len(result))
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)

    @pytest.mark.dbtest
    def test_fetch_uuid_filter_by_nominal_datetime(self, bdb_backend):
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

        result = self.query.execute(bdb_backend)
        assert(3 == len(result))
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)

    @pytest.mark.dbtest
    def test_fetch_uuid_filter_by_source_PLC(self, bdb_backend):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.eq(
            expr.attribute("_bdb/source:PLC", "string"),
            expr.literal("Harku")
        )

        result = self.query.execute(bdb_backend)
        assert (2 == len(result))
        assert ({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert ({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_fetch_uuid_filter_by_source_name_or_source_name(self, bdb_backend):
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

        result = self.query.execute(bdb_backend)
        assert(5 == len(result))
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[2]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_fetch_uuid_filter_by_source_name_and_source_name(self, bdb_backend):
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

        result = self.query.execute(bdb_backend)
        assert(0 == len(result))
    
    @pytest.mark.dbtest
    def test_fetch_uuid_filter_by_source_name_like(self, bdb_backend):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.like(
            expr.attribute("_bdb/source_name", "string"),
            "eeh*",
        )

        result = self.query.execute(bdb_backend)
        assert(2 == len(result))
        assert({"uuid": self.files[1]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_order_by(self, bdb_backend):
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

        result = self.query.execute(bdb_backend)
        assert(7 == len(result))
        assert({"uuid": self.files[6]["bdb_uuid"]} == result[0])
        assert({"uuid": self.files[5]["bdb_uuid"]} == result[1])
        assert({"uuid": self.files[4]["bdb_uuid"]} == result[2])
        assert({"uuid": self.files[3]["bdb_uuid"]} == result[3])
        assert({"uuid": self.files[2]["bdb_uuid"]} == result[4])
        assert({"uuid": self.files[1]["bdb_uuid"]} == result[5])
        assert({"uuid": self.files[0]["bdb_uuid"]} == result[6])
    
    @pytest.mark.dbtest
    def test_limit(self, bdb_backend):
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

        result = self.query.execute(bdb_backend)
        assert(2 == len(result))
        assert({"uuid": self.files[6]["bdb_uuid"]} == result[0])
        assert({"uuid": self.files[5]["bdb_uuid"]} == result[1])
    
    @pytest.mark.dbtest
    def test_fetch_max_xsize(self, bdb_backend):
        self.query.fetch = {
            "max_xsize": expr.max(expr.attribute("where/xsize", "long"))
        }

        result = self.query.execute(bdb_backend)
        assert(1 == len(result))
        assert({"max_xsize": 8} == result[0])
    
    @pytest.mark.dbtest
    def test_fetch_count_ysize(self, bdb_backend):
        self.query.fetch = {
            "count_ysize" : expr.count(expr.attribute("where/ysize", "long"))
        }
        result = self.query.execute(bdb_backend)
        assert(1 == len(result))
        assert({"count_ysize": 10} == result[0])
    
    @pytest.mark.dbtest
    def test_fetch_min_ysize_filter_by_what_object(self, bdb_backend):
        self.query.fetch = {
            "min_ysize" : expr.min(expr.attribute("where/ysize", "long"))
        }
        self.query.filter = expr.eq(
            expr.attribute("what/object", "string"),
            expr.literal("CVOL")
        )

        result = self.query.execute(bdb_backend)
        assert(1 == len(result))
        assert({"min_ysize": 4} == result[0])
    
    @pytest.mark.dbtest
    def test_fetch_sum_xsize_group_by_source_name(self, bdb_backend):
        self.query.fetch = {
            "source": expr.attribute("_bdb/source_name", "string"),
            "sum_xsize": expr.sum(expr.attribute("where/xsize", "long"))
        }
        self.query.group = [
            expr.attribute("_bdb/source_name", "string")
        ]

        result = self.query.execute(bdb_backend)
        assert(4 == len(result))
        assert({"source": "eesur", "sum_xsize": 14} in result)
        assert({"source": "eehar", "sum_xsize": 8} in result)
        assert({"source": "sekrn", "sum_xsize": 14} in result)
        assert({"source": "sevax", "sum_xsize": 16} in result)
    
    @pytest.mark.dbtest
    def test_fetch_sum_xsize_group_by_source_name_filter_by_date(self, bdb_backend):
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

        result = self.query.execute(bdb_backend)
        assert(2 == len(result))
        assert({"source": "eesur", "sum_xsize": 7} in result)
        assert({"source": "eehar", "sum_xsize": 2} in result)
    
    @pytest.mark.dbtest
    def test_fetch_max_xsize_group_by_source_name(self, bdb_backend):
        self.query.fetch = {
            "source": expr.attribute("_bdb/source_name", "string"),
            "max_xsize": expr.max(expr.attribute("where/xsize", "long"))
        }
        self.query.group = [
            expr.attribute("_bdb/source_name", "string")
        ]

        result = self.query.execute(bdb_backend)
        assert(4 == len(result))
        assert({"source": "eesur", "max_xsize": 5} in result)
        assert({"source": "eehar", "max_xsize": 6} in result)
        assert({"source": "sekrn", "max_xsize": 7} in result)
        assert({"source": "sevax", "max_xsize": 8} in result)
    
    @pytest.mark.dbtest
    def test_fetch_max_xsize_min_ysize_group_by_source_name(self, bdb_backend):
        self.query.fetch = {
            "source": expr.attribute("_bdb/source_name", "string"),
            "max_xsize": expr.max(expr.attribute("where/xsize", "long")),
            "min_ysize": expr.min(expr.attribute("where/ysize", "long"))
        }
        self.query.group = [
            expr.attribute("_bdb/source_name", "string")
        ]

        result = self.query.execute(bdb_backend)
        assert(4 == len(result))
        assert({"source": "eesur", "max_xsize": 5, "min_ysize": 2} in result)
        assert({"source": "eehar", "max_xsize": 6, "min_ysize": 2} in result)
        assert({"source": "sekrn", "max_xsize": 7, "min_ysize": 7} in result)
        assert({"source": "sevax", "max_xsize": 8, "min_ysize": 8} in result)
    
    @pytest.mark.dbtest
    def test_fetch_missing_value_group_by_source_name(self, bdb_backend):
        self.query.fetch = {
            "source": expr.attribute("_bdb/source_name", "string"),
            "min_elangle": expr.min(expr.attribute("where/elangle", "double")),
        }
        self.query.group = [
            expr.attribute("_bdb/source_name", "string")
        ]

        result = self.query.execute(bdb_backend)
        assert(4 == len(result))
        assert({"source": "eesur", "min_elangle": None} in result)
        assert({"source": "eehar", "min_elangle": None} in result)
        assert({"source": "sekrn", "min_elangle": None} in result)
        assert({"source": "sevax", "min_elangle": None} in result)
    
    @pytest.mark.dbtest
    def test_fetch_uuid_filter_by_what_object_in(self, bdb_backend):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.in_(
            expr.attribute("what/object", "string"),
            expr.literal(["CVOL", "SCAN"])
        )

        result = self.query.execute(bdb_backend)
        assert(4 == len(result))
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
        assert({"uuid": self.files[5]["bdb_uuid"]} in result)
        assert({"uuid": self.files[6]["bdb_uuid"]} in result)
    
    @pytest.mark.dbtest
    def test_fetch_uuid_filter_by_xsize_not_in(self, bdb_backend):
        self.query.fetch = {
            "uuid": expr.attribute("_bdb/uuid", "string")
        }
        self.query.filter = expr.not_(
            expr.in_(
                expr.attribute("where/xsize", "long"),
                expr.literal([2, 3])
            )
        )
 
        result = self.query.execute(bdb_backend)

        assert(7 == len(result))
        assert({"uuid": self.files[0]["bdb_uuid"]} in result)
        assert({"uuid": self.files[3]["bdb_uuid"]} in result)
        assert({"uuid": self.files[4]["bdb_uuid"]} in result)
        assert({"uuid": self.files[5]["bdb_uuid"]} in result)
        assert({"uuid": self.files[5]["bdb_uuid"]} in result) # We have two datasets with same xsize?
        assert({"uuid": self.files[6]["bdb_uuid"]} in result)
        assert({"uuid": self.files[6]["bdb_uuid"]} in result) # We have two datasets with same xsize?

    @pytest.mark.dbtest
    def test_fetch_xsize_by_what_object_and_dataset_absolute(self, bdb_backend):
        self.query.fetch = {
            "xsize": expr.attribute("/dataset1/where/xsize", "long")
        }
        self.query.filter = expr.and_(
            expr.eq(expr.attribute("/dataset1/where/ysize", "long"),
                    expr.literal(2)),
            expr.eq(expr.attribute("what/object", "string"),
                    expr.literal("SCAN"))
        )

        result = self.query.execute(bdb_backend)
        assert(1 == len(result))
        assert(5 == result[0]['xsize'])

    @pytest.mark.dbtest
    def test_fetch_xsize_by_what_object_and_dataset_absolute_fetch(self, bdb_backend):
        self.query.fetch = {
            "xsize": expr.attribute("/dataset1/where/xsize", "long")
        }
        self.query.filter = expr.and_(
            expr.eq(expr.attribute("where/ysize", "long"),
                    expr.literal(2)),
            expr.eq(expr.attribute("what/object", "string"),
                    expr.literal("SCAN"))
        )

        result = self.query.execute(bdb_backend)
        assert(1 == len(result))
        assert(5 == result[0]['xsize'])

    @pytest.mark.dbtest
    def test_fetch_xsize_by_what_object_and_dataset_relative_fetch_and_filter(self, bdb_backend):
        self.query.fetch = {
            "xsize": expr.attribute("where/xsize", "long")
        }
        self.query.filter = expr.and_(
            expr.eq(expr.attribute("where/ysize", "long"),
                    expr.literal(2)),
            expr.eq(expr.attribute("what/object", "string"),
                    expr.literal("SCAN"))
        )

        result = self.query.execute(bdb_backend)
        assert(2 == len(result))
        assert(True == ((result[0]['xsize'] == 2 and result[1]['xsize'] == 5) or
                   (result[0]['xsize'] == 5 and result[1]['xsize'] == 2)))
        
    @pytest.mark.dbtest
    def test_fetch_nodata_by_what_object_and_dataset_relative_fetch_and_filter(self, bdb_backend):
        self.query.fetch = {
            "nodata": expr.attribute("data1/what/nodata", "double")
        }
        self.query.filter = expr.and_(
            expr.eq(expr.attribute("data1/what/nodata", "double"),
                    expr.literal(255)),
            expr.eq(expr.attribute("what/object", "string"),
                    expr.literal("SCAN"))
        )

        result = self.query.execute(bdb_backend)
        assert(3 == len(result))
        assert(255.0 == result[0]['nodata'])
        assert(255.0 == result[1]['nodata'])
        assert(255.0 == result[2]['nodata'])

    @pytest.mark.dbtest
    def test_fetch_nodata_by_what_object_and_dataset_absolue_fetch_nomatch(self, bdb_backend):
        self.query.fetch = {
            "nodata": expr.attribute("data1/what/nodata", "double")
        }
        self.query.filter = expr.and_(
            expr.eq(expr.attribute("/data1/what/nodata", "double"),
                    expr.literal(255)),
            expr.eq(expr.attribute("what/object", "string"),
                    expr.literal("SCAN"))
        )

        result = self.query.execute(bdb_backend)
        assert(0 == len(result))

    @pytest.mark.dbtest
    def test_fetch_nodata_by_what_object_and_dataset_relative_many_match(self, bdb_backend):
        self.query.fetch = {
            "nodata": expr.attribute("what/nodata", "double")
        }
        self.query.filter = expr.and_(
            expr.gt(expr.attribute("what/nodata", "double"),
                    expr.literal(200)),
            expr.eq(expr.attribute("what/object", "string"),
                    expr.literal("SCAN"))
        )

        result = self.query.execute(bdb_backend)
        assert(12 == len(result))
