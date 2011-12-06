import httplib

from mock import Mock

from nose.tools import eq_, raises

from werkzeug.test import EnvironBuilder

from baltrad.bdb.backend import Backend, DuplicateEntry
from baltrad.bdb.web import handler
from baltrad.bdb.web.util import (
    HttpConflict,
    HttpNotFound,
    Request,
    RequestContext,
)
from baltrad.bdb.oh5 import Metadata, Source

class TestFileHandlers(object):
    def setup(self):
        self.backend = Mock(spec_set=Backend)
        self.ctx = RequestContext(None, self.backend)
    
    def create_request(self, method, data):
        return EnvironBuilder(
            method=method,
            data=data,
        ).get_request(Request)

    def test_add_file(self):
        self.ctx.request = self.create_request("POST", data="filecontent")
        metadata = Mock(spec_set=Metadata)
        self.backend.get_file_metadata.return_value = metadata
        metadata.json_repr.return_value = {"key": "value"}
        metadata.bdb_uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8"

        self.backend.store_file.return_value = metadata
        response = handler.add_file(self.ctx)
        self.backend.store_file.assert_called_once()
        eq_(httplib.CREATED, response.status_code)
        eq_("/file/6ba7b810-9dad-11d1-80b4-00c04fd430c8", response.headers["Location"])
        eq_('{"metadata": {"key": "value"}}', response.data)
    
    @raises(HttpConflict)
    def test_add_file_duplicate(self):
        self.ctx.request = self.create_request("POST",
            data='filecontent'
        )

        metadata = Metadata()
        metadata.bdb_uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
        self.backend.store_file.side_effect = DuplicateEntry()
        handler.add_file(self.ctx)
    
    def test_get_file(self):
        self.backend.get_file.return_value = "filecontent"
        
        uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
        response = handler.get_file(self.ctx, uuid)
        self.backend.get_file.assert_call_once_with(uuid)

        eq_(httplib.OK, response.status_code)
        eq_('filecontent', response.data)
    
    @raises(HttpNotFound)
    def test_get_file_nx(self):
        self.backend.get_file.return_value = None

        uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
        handler.get_file(self.ctx, uuid)
    
    def test_get_file_metadata(self):
        metadata = Mock(spec_set=Metadata)
        self.backend.get_file_metadata.return_value = metadata
        metadata.json_repr.return_value = {"key": "value"}

        uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
        response = handler.get_file_metadata(self.ctx, uuid)
        eq_(httplib.OK, response.status_code)
        eq_('{"metadata": {"key": "value"}}', response.data)

    @raises(HttpNotFound)
    def test_get_file_metadata_nx(self):
        self.backend.get_file_metadata.return_value = None
        uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
        handler.get_file_metadata(self.ctx, uuid)
    
    def test_remove_file(self):
        self.backend.remove_file.return_value = True
        uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
        response = handler.remove_file(self.ctx, uuid)
        eq_(httplib.NO_CONTENT, response.status_code)
 
    @raises(HttpNotFound)
    def test_remove_file_nx(self):
        self.backend.remove_file.return_value = False
        uuid = "6ba7b810-9dad-11d1-80b4-00c04fd430c8"
        handler.remove_file(self.ctx, uuid)
    
    def test_remove_all_files(self):
        response = handler.remove_all_files(self.ctx)
        eq_(httplib.NO_CONTENT, response.status_code)

class TestSourceHandlers(object):
    def setup(self):
        self.backend = Mock(spec_set=Backend)
        self.ctx = RequestContext(None, self.backend)

    def test_get_sources(self):
        self.backend.get_sources.return_value = [
            Source.from_string("_name:src1,key1:value1"),
            Source.from_string("_name:src2,key2:value2"),
        ]
        
        response = handler.get_sources(self.ctx)
        eq_(httplib.OK, response.status_code) 
        expected = (
            '{"sources": ['
                '{"key1": "value1", "_name": "src1"}, '
                '{"key2": "value2", "_name": "src2"}'
            ']}'
        )
        eq_(expected, response.data)


class TestQueryHandlers(object):
    def setup(self):
        self.backend = Mock(spec_set=Backend)
        self.ctx = RequestContext(None, self.backend)

    def create_request(self, method, data):
        return EnvironBuilder(
            method=method,
            data=data,
        ).get_request(Request)
    
    def test_query_file(self):
        self.ctx.request = self.create_request("POST",
            data=(
                '{'
                    '"filter": null,'
                    '"order": [],'
                    '"limit": 2,'
                    '"skip": 1'
                '}'
            )
        )
        self.backend.execute_file_query.return_value = [
            "00000000-0000-0000-0004-000000000001",
            "00000000-0000-0000-0004-000000000002"
        ]

        response = handler.query_file(self.ctx)
        eq_(httplib.OK, response.status_code)
        expected = (
            '{"rows": ['
                '"00000000-0000-0000-0004-000000000001", '
                '"00000000-0000-0000-0004-000000000002"'
            ']}'
        )
        eq_(expected, response.data)
    
    def test_query_attribute(self):
        self.ctx.request = self.create_request("POST",
            data=(
                '{'
                    '"fetch": {"uuid": ["attr", "file:uuid", "string"]},'
                    '"filter": null,'
                    '"distinct": false,'
                    '"order": [],'
                    '"group": [],'
                    '"limit": 2,'
                    '"skip": 1'
                '}'
            )
        )

        self.backend.execute_attribute_query.return_value = [
            {"uuid": "00000000-0000-0000-0004-000000000001"},
            {"uuid": "00000000-0000-0000-0004-000000000002"}
        ]

        response = handler.query_attribute(self.ctx)
        eq_(httplib.OK, response.status_code)
        expected = (
            '{"rows": ['
                '{"uuid": "00000000-0000-0000-0004-000000000001"}, '
                '{"uuid": "00000000-0000-0000-0004-000000000002"}'
            ']}'
        )
        eq_(expected, response.data)
