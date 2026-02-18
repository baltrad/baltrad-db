# -*- encoding: utf8 -*-

import collections
import datetime
import os

import pytest

from baltrad.bdbcommon.oh5 import (
    Attribute,
    Group,
    Metadata,
    Source,
)

class TestMetadata(object):
    def setup_method(self, method):
        self.meta = Metadata()
    
    def test_add_node_group(self):
        grp = self.meta.add_node("/", Group("group"))
        assert(grp.name == "group")
        assert(self.meta.root() == grp.parent)
    
    def test_add_node_attribute(self):
        attr = self.meta.add_node("/", Attribute("attr", "value"))
        assert(attr.name == "attr")
        assert(attr.value == "value")
    
    def test_find_node(self):
        self.meta.add_node("/", Group("a"))
        self.meta.add_node("/a", Group("b"))
        c = self.meta.add_node("/a/b", Group("c"))

        assert(c == self.meta.find_node("/a/b/c"))
    
    def test_find_node_root(self):
        assert(self.meta.root() == self.meta.find_node("/"))
    
    def test_node_nx(self):
        with pytest.raises(LookupError):
            self.meta.node("/a/b/c")

    def test_find_node_nx(self):
        assert(self.meta.find_node("/a/b") is None)
    
    def test_find_root(self):
        assert(self.meta.find_node("/a/b") is None)

    def test_iterator(self):
        self.meta.add_node("/", Group("a"))
        self.meta.add_node("/", Group("b"))
        self.meta.add_node("/a", Group("b"))
        self.meta.add_node("/a/b", Group("c"))

        expected = ["/", "/a", "/b", "/a/b", "/a/b/c"]
        nodepaths = [node.path() for node in self.meta.iternodes()]
        assert(nodepaths == expected)

    def test_iterator_2(self):
        self.meta.add_node("/", Group("a"))
        self.meta.add_node("/", Group("b"))
        self.meta.add_node("/", Group("ab"))
        self.meta.add_node("/", Group("ba"))
        self.meta.add_node("/a", Group("b"))
        self.meta.add_node("/a/b", Group("c"))

        expected = ["/", "/a", "/ab", "/b", "/ba", "/a/b", "/a/b/c"]
        nodepaths = [node.path() for node in self.meta.iternodes()]
        assert(nodepaths == expected)

    def test_iterator_3(self):
        self.meta.add_node("/", Group("a"))
        self.meta.add_node("/", Group("b"))
        self.meta.add_node("/", Attribute("a1", 1))
        self.meta.add_node("/", Group("ba"))
        self.meta.add_node("/a", Attribute("a2", 1))
        self.meta.add_node("/a", Group("c"))

        expected = ["/", "/a", "/a1", "/b", "/ba", "/a/a2", "/a/c"]
        nodepaths = [node.path() for node in self.meta.iternodes()]
        assert(nodepaths == expected)
            
    def test_source(self):
        self.meta.add_node("/", Group("what"))
        self.meta.add_node(
            "/what",
            Attribute("source", "NOD:eesur,PLC:Sürgavere")
        )

        assert(self.meta.source() == {"NOD": "eesur", "PLC": "Sürgavere"})

    def test_json_repr_empty(self):
        expected = [{"path": "/", "type": "group"}]

        assert(self.meta.json_repr() == expected)

    def test_json_repr(self):
        self.meta.add_node("/", Group("a"))
        self.meta.add_node("/a", Group("b"))
        self.meta.add_node("/", Attribute("attr1", 1))
        self.meta.add_node("/a", Attribute("attr2", 2))
        
        expected = [
            {"path": "/", "type": "group"},
            {"path": "/a", "type": "group"},
            {"path": "/attr1", "type": "attribute", "value": 1},
            {"path": "/a/attr2", "type": "attribute", "value": 2},
            {"path": "/a/b", "type": "group"}
        ]

        assert(self.meta.json_repr() == expected)

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

    def setup_method(self, method):
        self.meta = Metadata()

    def check_get_property(self, propdef):
        group_path, attr_name = os.path.split(propdef.oh5attr)
        group_parent_path, group_name = os.path.split(group_path)
        self.meta.add_node(group_parent_path, Group(group_name))
        self.meta.add_node(group_path, Attribute(attr_name, propdef.oh5value))

        assert(getattr(self.meta, propdef.attrname) == propdef.pyvalue)
    
    def test_get_property(self):
        for propdef in self.propdefs:
            self.check_get_property(propdef)

    def check_get_property_nx(self, propdef):
        assert(getattr(self.meta, propdef.attrname) is None)
    
    def test_get_property_nx(self):
        for propdef in self.propdefs:
            self.check_get_property_nx(propdef)
    
    def check_set_property(self, propdef):
        setattr(self.meta, propdef.attrname, propdef.pyvalue)
        assert(self.meta.node(propdef.oh5attr).value == propdef.oh5value)

    def test_set_property(self):
        for propdef in self.propdefs:
            self.check_set_property(propdef)

class TestSource(object):
    def test_to_string(self):
        src = Source()
        src["key1"] = "value1"
        src["key2"] = "value2"
        src["_key3"] = "value3"
        assert(src.to_string() == "_key3:value3,key1:value1,key2:value2")
    
    def test_dict_copy(self):
        copy = dict(Source("foo", values={"bar": "baz"}))
        assert(copy == {"bar": "baz"})
    
    def test_get(self):
        src = Source()
        assert(src.get("qwe") is None)
        assert(src.get("qwe", "asd") == "asd")

    def test_from_string(self):
        src = Source.from_string("asd:qwe,key:value")
        assert(src["asd"] == "qwe")
        assert(src["key"] == "value")
    
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
        assert(result[0] == Source("ee", values={"CCCC": "EEMH", "ORG": "231"},))
        assert(result[1] == Source("eehar", values={"NOD": "eehar","PLC": "Harku","RAD": "EE40","WMO": "26038"}, parent = "ee"))
        assert(result[2] == Source("eesur", values={"NOD": "eesur","PLC": u"Sürgavere","RAD": "EE41","WMO": "26232",}, parent = "ee"))
        assert(result[3] == Source("fi", values={"CCCC": "EFKL","ORG": "86",}))
        assert(result[4] == Source("fivan", values={"NOD": "fivan","PLC": "Vantaa","RAD": "FI42","WMO": "02975",}, parent = "fi"))
        assert(len(result) == 5)
