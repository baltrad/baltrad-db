import datetime

#from mock import patch, sentinel, Mock
from nose.tools import eq_

from baltrad.bdbcommon.oh5 import (
    Attribute,
    Group,
    Metadata,
    MetadataHasher
)

class TestMetadataHasher(object):
    def setUp(self):
        self.hasher = MetadataHasher()

    def test_attribute_string(self):
        pass

    def test_concrete_digest1(self):
        meta = Metadata();
        meta.add_node("/", Group("what"))
        meta.add_node("/what", Attribute("source", "WMO:02606"))
        meta.add_node("/what", Attribute("date", datetime.date(2000, 1, 2)))
        meta.add_node("/what", Attribute("time", datetime.time(12, 5)))
        meta.add_node("/what", Attribute("object", "pvol"))

        eq_(
            "2fe7e00e5c5fb6fa98c8b07154d1e4d2fcde7811",
            self.hasher.hash(meta)
        )
    
    def test_concrete_digest2(self):
        meta = Metadata();
        meta.add_node("/", Group("what"))
        meta.add_node("/what", Attribute("source", "WMO:02606"))
        meta.add_node("/what", Attribute("date", datetime.date(2001, 1, 2)))
        meta.add_node("/what", Attribute("time", datetime.time(12, 5)))
        meta.add_node("/what", Attribute("object", "pvol"))
        eq_(
            "469fa750ff4f575f1c30c794bb80e16a6a99ed2c",
            self.hasher.hash(meta)
        )
