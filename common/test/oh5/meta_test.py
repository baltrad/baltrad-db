# -*- encoding: utf8 -*-

import collections
import datetime
import os

from nose.tools import eq_, raises

from baltrad.bdbcommon.oh5 import Metadata, Source

class TestMetadata(object):
    def setUp(self):
        self.meta = Metadata()
    
    def test_add_group(self):
        grp = self.meta.add_group("/group")
        eq_(grp.name, "group")
        eq_(self.meta.root(), grp.parent)
    
    def test_add_attribute(self):
        attr = self.meta.add_attribute("attr", "value")
        eq_(attr.name, "attr")
        eq_(attr.value, "value")
    
    def test_find_node(self):
        self.meta.add_group("/a")
        self.meta.add_group("/a/b")
        self.meta.add_group("/a/b/c")
        c = self.meta.add_group("/a/b/c")

        eq_(c, self.meta.find_node("/a/b/c"))
    
    def test_node_root(self):
        eq_(self.meta.root(), self.meta.find_node("/"))
    
    @raises(LookupError)
    def test_node_nx(self):
        self.meta.node("/a/b/c")

    def test_find_node_nx(self):
        eq_(None, self.meta.find_node("/a/b"))
    
    def test_find_root(self):
        eq_(None, self.meta.find_node("/a/b"))

    def test_iterator(self):
       self.meta.add_group("/a")
       self.meta.add_group("/b")
       self.meta.add_group("/a/b")
       self.meta.add_group("/a/b/c")

       expected = ["/", "/a", "/b", "/a/b", "/a/b/c"]
       nodepaths = [node.path() for node in self.meta.iternodes()]
       eq_(expected, nodepaths)
            
    def test_source(self):
        self.meta.add_group("/what")
        self.meta.add_attribute("/what/source", "NOD:eesur,PLC:Sürgavere")

        eq_({"NOD": "eesur", "PLC": "Sürgavere"}, self.meta.source())

    def test_json_repr_empty(self):
        expected = [{"path": "/", "type": "group"}]

        eq_(expected, self.meta.json_repr())

    def test_json_repr(self):
        self.meta.add_group("/a")
        self.meta.add_group("/a/b")
        self.meta.add_attribute("/attr1", 1)
        self.meta.add_attribute("/a/attr2", 2)
        
        expected = [
            {"path": "/", "type": "group"},
            {"path": "/a", "type": "group"},
            {"path": "/attr1", "type": "attribute", "value": 1},
            {"path": "/a/b", "type": "group"},
            {"path": "/a/attr2", "type": "attribute", "value": 2}
        ]

        eq_(expected, self.meta.json_repr())

class TestMetadataAttributeShortcuts(object):
    PropDef = collections.namedtuple(
        "PropDef",
        ["attrname", "oh5attr", "pyvalue", "oh5value"]
    )

    propdefs = [
        PropDef("what_object", "/what/object",
                "pvol", "pvol"),
        PropDef("what_source", "/what/source",
                "NOD:eesur", "NOD:eesur"),
        PropDef("what_date", "/what/date",
                datetime.date(2000, 1, 2), "20000102"),
        PropDef("what_time", "/what/time",
                datetime.time(13, 14, 15), "131415"),
        PropDef("bdb_uuid", "/_bdb/uuid",
                "6ba7b810-9dad-11d1-80b4-00c04fd430c8",
                "6ba7b810-9dad-11d1-80b4-00c04fd430c8"),
        PropDef("bdb_metadata_hash", "/_bdb/metadata_hash",
                "abcdef12345", "abcdef12345"),
        PropDef("bdb_file_size", "/_bdb/file_size",
                1234, 1234),
        PropDef("bdb_source", "/_bdb/source",
                "NOD:eesur", "NOD:eesur"),
        PropDef("bdb_source_name", "/_bdb/source_name",
                "eesur", "eesur"),
        PropDef("bdb_stored_date", "/_bdb/stored_date",
                datetime.date(2000, 1, 2), "20000102"),
        PropDef("bdb_stored_time", "/_bdb/stored_time",
                datetime.time(13, 14, 15), "131415"),
    ]

    def setUp(self):
        self.meta = Metadata()

    def check_get_property(self, propdef):
        self.meta.add_group(os.path.dirname(propdef.oh5attr))
        self.meta.add_attribute(propdef.oh5attr, propdef.oh5value)

        eq_(propdef.pyvalue, getattr(self.meta, propdef.attrname))
    
    def test_get_property(self):
        for propdef in self.propdefs:
            yield self.check_get_property, propdef

    def check_get_property_nx(self, propdef):
        eq_(None, getattr(self.meta, propdef.attrname))
    
    def test_get_property_nx(self):
        for propdef in self.propdefs:
            yield self.check_get_property_nx, propdef
    
    def check_set_property(self, propdef):
        setattr(self.meta, propdef.attrname, propdef.pyvalue)
        eq_(propdef.oh5value, self.meta.node(propdef.oh5attr).value)

    def test_set_property(self):
        for propdef in self.propdefs:
            yield self.check_set_property, propdef

class TestSource(object):
    def test_to_string(self):
        src = Source()
        src["key1"] = "value1"
        src["key2"] = "value2"
        src["_key3"] = "value3"
        eq_("_key3:value3,key1:value1,key2:value2", src.to_string())
    
    def test_get(self):
        src = Source()
        eq_(None, src.get("qwe"))
        eq_("asd", src.get("qwe", "asd"))

    def test_from_string(self):
        src = Source.from_string("asd:qwe,key:value")
        eq_("qwe", src["asd"])
        eq_("value", src["key"])
    
    def test_from_rave_xml(self):
        xml = """<?xml version='1.0' encoding='utf-8'?>
            <radar-db>
              <ee CCCC="EEMH" org="231">
                <eehar nod="eehar" plc="Harku" rad="EE40" wmo="26038"></eehar>
                <eesur nod="eesur" plc="Sürgavere" rad="EE41" wmo="26232"></eesur>
              </ee>
             <fi CCCC="EFKL" org="86">
                <fivan nod="fivan" plc="Vantaa" rad="FI42" wmo="02975"></fivan>
              </fi>
            </radar-db>
        """
        result = Source.from_rave_xml(xml)
        eq_(
            Source("ee",
                values={"CCCC": "EEMH", "ORG": "231"}
            ),
            result[0]
        )
        eq_(
            Source("eehar",
                values={
                    "NOD": "eehar",
                    "PLC": "Harku",
                    "RAD": "EE40",
                    "WMO": "26038"
                }
            ),
            result[1]
        )
        eq_(
            Source("eesur",
                values={
                    "NOD": "eesur",
                    "PLC": u"Sürgavere",
                    "RAD": "EE41",
                    "WMO": "26232",
                }
            ),
            result[2]
        )
        eq_(
            Source("fi",
                values= {
                    "CCCC": "EFKL",
                    "ORG": "86",
            }),
            result[3]
        )
        eq_(
            Source("fivan",
                values={
                    "NOD": "fivan",
                    "PLC": "Vantaa",
                    "RAD": "FI42",
                    "WMO": "02975",
                },
            ),
            result[4]
        )
        eq_(5, len(result))
