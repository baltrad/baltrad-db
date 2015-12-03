import httplib

import mock
from nose.tools import eq_, raises

from werkzeug.test import EnvironBuilder

from baltrad.bdbserver.backend import (
    Backend,
    SourceManager,
    FilterManager,
    DuplicateEntry,
    IntegrityError
)
from baltrad.bdbserver.web import handler
from baltrad.bdbserver.web.util import (
    HttpConflict,
    HttpForbidden,
    HttpNotFound,
    Request,
    RequestContext,
)
from baltrad.bdbcommon import expr, filter
from baltrad.bdbcommon.oh5 import Metadata, Source

class TestFileHandlers(object):
    def setup(self):
        self.backend = mock.Mock(spec_set=Backend)
        self.ctx = RequestContext(None, self.backend)
    
    def create_request(self, method, data):
        return EnvironBuilder(
            method=method,
            data=data,
        ).get_request(Request)

    def test_add_file(self):
        self.ctx.request = self.create_request("POST", data="filecontent")
        metadata = mock.Mock(spec_set=Metadata)
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
        metadata = mock.Mock(spec_set=Metadata)
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
        self.ctx.enable_remove_all_files = True
        response = handler.remove_all_files(self.ctx)
        eq_(httplib.NO_CONTENT, response.status_code)
    
    @raises(HttpForbidden)
    def test_remove_all_files_not_enabled(self):
        handler.remove_all_files(self.ctx)

class TestSourceHandlers(object):
    def setup(self):
        self.backend = mock.Mock(spec_set=Backend)
        self.source_manager = mock.Mock(spec_set=SourceManager)
        self.backend.get_source_manager.return_value = self.source_manager
        self.ctx = RequestContext(None, self.backend)

    def create_request(self, method, data):
        return EnvironBuilder(
            method=method,
            data=data,
        ).get_request(Request)

    def test_get_sources(self):
        self.source_manager.get_sources.return_value = [
            Source("src1", {"key1": "value1"}),
            Source("src2", {"key2": "value2"}, "src1"),
        ]
        
        response = handler.get_sources(self.ctx)
        eq_(httplib.OK, response.status_code) 
        expected = (
            '{"sources": ['
                '{"values": {"key1": "value1"}, "name": "src1", "parent": null}, '
                '{"values": {"key2": "value2"}, "name": "src2", "parent": "src1"}'
            ']}'
        )
        eq_(expected, response.data)
    
    def test_add_source(self):
        self.ctx.request = self.create_request("POST",
            data='{"source": {"name": "foo", "values": {"bar": "baz"}}}',
        )

        response = handler.add_source(self.ctx)

        self.source_manager.add_source.assert_called_with(
            Source("foo", values={"bar": "baz"})
        )
        eq_(httplib.CREATED, response.status_code)
        eq_("/source/foo", response.headers["Location"])
    
    def test_add_source_duplicate(self):
        pass
    
    def test_update_source(self):
        self.ctx.request = self.create_request("PUT",
            data='{"source": {"name": "foo", "values": {"bar": "baz"}}}',
        )

        response = handler.update_source(self.ctx)
        self.source_manager.update_source.assert_called_with(
            Source("foo", values={"bar": "baz"})
        )
        eq_(httplib.NO_CONTENT, response.status_code)
        eq_("/source/foo", response.headers["Location"])
    
    def test_update_source_not_found(self):
        self.ctx.request = self.create_request("PUT",
            data='{"source": {"name": "foo", "values": {"bar": "baz"}}}',
        )
        self.source_manager.update_source.side_effect = LookupError()

        response = handler.update_source(self.ctx)
        eq_(httplib.NOT_FOUND, response.status_code)
    
    def test_remove_source(self):
        self.ctx.request = self.create_request("DELETE", data="")
        self.source_manager.remove_source.return_value = True

        response = handler.remove_source(self.ctx, "foo")
        self.source_manager.remove_source.assert_called_with("foo")
        eq_(httplib.NO_CONTENT, response.status_code)
    
    def test_remove_source_not_found(self):
        self.ctx.request = self.create_request("DELETE", data="")
        self.source_manager.remove_source.return_value = False

        response = handler.remove_source(self.ctx, "foo")
        self.source_manager.remove_source.assert_called_with("foo")
        eq_(httplib.NOT_FOUND, response.status_code)
    
    def test_remove_source_associated_files(self):
        self.ctx.request = self.create_request("DELETE", data="")
        self.source_manager.remove_source.side_effect = IntegrityError()

        response = handler.remove_source(self.ctx, "foo")
        self.source_manager.remove_source.assert_called_with("foo")
        eq_(httplib.CONFLICT, response.status_code)
        
class TestFilterHandlers(object):
    def setup(self):
        self.backend = mock.Mock(spec_set=Backend)
        self.ctx = RequestContext(None, self.backend)
        self.filter_manager = mock.Mock(spec_set=FilterManager)
        self.backend.get_filter_manager.return_value = self.filter_manager
        self.filter1 = filter.Filter(
            "filter1",
            expr.eq(
                expr.attribute("what/object", "string"),
                expr.literal("PVOL")
            )
        )
        self.filter1_repr = (
            '{"filter": {'
                '"expression": ['
                    '"list", ["symbol", "="], '
                            '["list", ["symbol", "attr"], '
                                     '"what/object", '
                                     '"string"'
                            '], '
                            '"PVOL"'
                '], '
                '"name": "filter1"'
            '}}'
        )

    def create_request(self, method, data):
        return EnvironBuilder(
            method=method,
            data=data,
        ).get_request(Request)
    
    def test_get_filters(self):
        self.ctx.request = self.create_request("GET", data="")
        self.filter_manager.get_filter_names.return_value = [
            "filter1", "filter2"
        ]

        response = handler.get_filters(self.ctx)
        eq_(httplib.OK, response.status_code)
        expected = '{"filters": ["filter1", "filter2"]}'
        eq_(expected, response.data)
    
    def test_get_filter(self):
        self.ctx.request = self.create_request("GET", data="")
        self.filter_manager.get_filter.return_value = self.filter1

        response = handler.get_filter(self.ctx, "filter1")
        eq_(httplib.OK, response.status_code)
        self.filter_manager.get_filter.assert_called_once_with("filter1")
        eq_(self.filter1_repr, response.data)
    
    def test_add_filter(self):
        self.ctx.request = self.create_request("POST", data=self.filter1_repr)
        
        response = handler.add_filter(self.ctx)
        eq_(httplib.CREATED, response.status_code)
        eq_("/filter/filter1", response.headers["Location"])
        self.filter_manager.add_filter.assert_called_once_with(self.filter1)
    
    def test_update_filter(self):
        self.ctx.request = self.create_request("PUT", data=self.filter1_repr)

        filter2 = filter.Filter(
            "filter2",
            self.filter1.expression,
        )

        response = handler.update_filter(self.ctx, "filter2")
        eq_(httplib.NO_CONTENT, response.status_code)
        self.filter_manager.update_filter.assert_called_once_with(filter2)
     
    @raises(HttpNotFound)
    def test_update_filter_not_found(self):
        self.ctx.request = self.create_request("PUT", data=self.filter1_repr)
        self.filter_manager.update_filter.side_effect = LookupError()

        handler.update_filter(self.ctx, "filter2")
    
    def test_remove_filter(self):
        self.ctx.request = self.create_request("DELETE", data="")
        self.filter_manager.remove_filter.return_value = True

        response = handler.remove_filter(self.ctx, "filter1")
        eq_(httplib.NO_CONTENT, response.status_code)
        self.filter_manager.remove_filter.assert_called_once_with("filter1")
    
    @raises(HttpNotFound)
    def test_remove_filter_not_found(self):
        self.ctx.request = self.create_request("DELETE", data="")
        self.filter_manager.remove_filter.return_value = False

        handler.remove_filter(self.ctx, "filter1")

    
class TestQueryHandlers(object):
    def setup(self):
        self.backend = mock.Mock(spec_set=Backend)
        self.ctx = RequestContext(None, self.backend)

    def create_request(self, method, data):
        return EnvironBuilder(
            method=method,
            data=data,
        ).get_request(Request)
    
    @mock.patch("baltrad.bdbcommon.expr.unwrap_json")
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
    
    @mock.patch("baltrad.bdbcommon.expr.unwrap_json")
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
