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
            "0833a94578041a8177afb30ee1e7ac0a660be043",
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
            "91176508177e2acc5638faec441a925a268700ae",
            self.hasher.hash(meta)
        )
