import httplib

from mock import Mock, patch

from nose.tools import eq_, raises

from werkzeug.test import EnvironBuilder

from baltrad.bdbserver.backend import (
    Backend,
    DuplicateEntry,
    IntegrityError
)
from baltrad.bdbserver.web import handler
from baltrad.bdbserver.web.util import (
    HttpConflict,
    HttpNotFound,
    Request,
    RequestContext,
)
from baltrad.bdbcommon.oh5 import Metadata, Source

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

    def create_request(self, method, data):
        return EnvironBuilder(
            method=method,
            data=data,
        ).get_request(Request)

    def test_get_sources(self):
        self.backend.get_sources.return_value = [
            Source("src1", {"key1": "value1"}),
            Source("src2", {"key2": "value2"}),
        ]
        
        response = handler.get_sources(self.ctx)
        eq_(httplib.OK, response.status_code) 
        expected = (
            '{"sources": ['
                '{"values": {"key1": "value1"}, "name": "src1"}, '
                '{"values": {"key2": "value2"}, "name": "src2"}'
            ']}'
        )
        eq_(expected, response.data)
    
    def test_add_source(self):
        self.ctx.request = self.create_request("POST",
            data='{"source": {"name": "foo", "values": {"bar": "baz"}}}',
        )

        response = handler.add_source(self.ctx)

        self.backend.add_source.assert_called_with(Source("foo", values={"bar": "baz"}))
        eq_(httplib.CREATED, response.status_code)
        eq_("/source/foo", response.headers["Location"])
    
    def test_add_source_duplicate(self):
        pass
    
    def test_update_source(self):
        self.ctx.request = self.create_request("PUT",
            data='{"source": {"name": "foo", "values": {"bar": "baz"}}}',
        )

        response = handler.update_source(self.ctx, "qaz")
        self.backend.update_source.assert_called_with(
            "qaz", Source("foo", values={"bar": "baz"})
        )
        eq_(httplib.NO_CONTENT, response.status_code)
        eq_("/source/qaz", response.headers["Location"])
    
    def test_update_source_not_found(self):
        self.ctx.request = self.create_request("PUT",
            data='{"source": {"name": "foo", "values": {"bar": "baz"}}}',
        )
        self.backend.update_source.side_effect = LookupError()

        response = handler.update_source(self.ctx, "qaz")
        eq_(httplib.NOT_FOUND, response.status_code)
    
    def test_update_source_duplicate_name(self):
        self.ctx.request = self.create_request("PUT",
            data='{"source": {"name": "foo", "values": {"bar": "baz"}}}',
        )
        self.backend.update_source.side_effect = DuplicateEntry()

        response = handler.update_source(self.ctx, "qaz")
        eq_(httplib.CONFLICT, response.status_code)
    
    def test_remove_source(self):
        self.ctx.request = self.create_request("DELETE", data="")
        self.backend.remove_source.return_value = True

        response = handler.remove_source(self.ctx, "foo")
        self.backend.remove_source.assert_called_with("foo")
        eq_(httplib.NO_CONTENT, response.status_code)
    
    def test_remove_source_not_found(self):
        self.ctx.request = self.create_request("DELETE", data="")
        self.backend.remove_source.return_value = False

        response = handler.remove_source(self.ctx, "foo")
        self.backend.remove_source.assert_called_with("foo")
        eq_(httplib.NOT_FOUND, response.status_code)
    
    def test_remove_source_associated_files(self):
        self.ctx.request = self.create_request("DELETE", data="")
        self.backend.remove_source.side_effect = IntegrityError()

        response = handler.remove_source(self.ctx, "foo")
        self.backend.remove_source.assert_called_with("foo")
        eq_(httplib.CONFLICT, response.status_code)
        

class TestQueryHandlers(object):
    def setup(self):
        self.backend = Mock(spec_set=Backend)
        self.ctx = RequestContext(None, self.backend)

    def create_request(self, method, data):
        return EnvironBuilder(
            method=method,
            data=data,
        ).get_request(Request)
    
    @patch("baltrad.bdbcommon.expr.unwrap_json")
    def test_query_file(self, unwrap_json):
        self.ctx.request = self.create_request("POST",
            data=(
                '{'
                    '"filter": "mockfilter",'
                    '"order": ["mockorder1", "mockorder2"],'
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
        eq_(
            [
                (("mockfilter",), {}),
                (("mockorder1",), {}),
                (("mockorder2",), {}),
            ],
            unwrap_json.call_args_list
        )
    
    @patch("baltrad.bdbcommon.expr.unwrap_json")
    def test_query_attribute(self, unwrap_json):
        self.ctx.request = self.create_request("POST",
            data=(
                '{'
                    '"fetch": {"fetch1": "mockfetch1", "fetch2": "mockfetch2"},'
                    '"filter": "mockfilter",'
                    '"distinct": false,'
                    '"order": ["mockorder1", "mockorder2"],'
                    '"group": ["mockgroup1", "mockgroup2"],'
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
        eq_(
            [
                (("mockfetch2",), {}),
                (("mockfetch1",), {}),
                (("mockfilter",), {}),
                (("mockorder1",), {}),
                (("mockorder2",), {}),
                (("mockgroup1",), {}),
                (("mockgroup2",), {}),
            ],
            unwrap_json.call_args_list, 
        )