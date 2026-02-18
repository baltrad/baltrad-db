# Copyright 2010-2011 Estonian Meteorological and Hydrological Institute
# 
# This file is part of baltrad-db.
# 
# baltrad-db is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# baltrad-db is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.

import uuid

import pytest

from baltrad.bdbserver.web.routing import URL_MAP, get_handler

from werkzeug.exceptions import MethodNotAllowed

class TestFileRoutes(object):
    @classmethod
    def setup_class(cls):
        cls.adapter = URL_MAP.bind("/")
        cls.uuid = uuid.uuid4()
    
    def test_fileroot_GET(self):
        with pytest.raises(MethodNotAllowed):
            self.adapter.match("/file/", "GET")
    
    def test_fileroot_POST(self):
        endpoint, args = self.adapter.match("/file/", "POST")
        assert("handler.add_file"==endpoint)

    def test_fileroot_PUT(self):
        with pytest.raises(MethodNotAllowed):
            self.adapter.match("/file/", "PUT")
     
    def test_fileroot_DELETE(self):
        endpoint, args = self.adapter.match("/file/", "DELETE")
        assert("handler.remove_all_files"==endpoint)
    
    def test_file_GET(self):
        endpoint, args = self.adapter.match("/file/" + str(self.uuid), "GET")
        assert("handler.get_file"==endpoint)
        assert(self.uuid==args["uuid"])
     
    def test_file_PUT(self):
        with pytest.raises(MethodNotAllowed):
            endpoint, args = self.adapter.match("/file/" + str(self.uuid), "PUT")

    def test_file_POST(self):
        with pytest.raises(MethodNotAllowed):
            endpoint, args = self.adapter.match("/file/" + str(self.uuid), "POST")
    
    def test_file_DELETE(self):
        endpoint, args = self.adapter.match("/file/" + str(self.uuid), "DELETE")
        assert("handler.remove_file"==endpoint)
        assert(self.uuid==args["uuid"])
    
    def test_file_metadata_GET(self):
        url = "/file/" + str(self.uuid) + "/metadata"
        endpoint, args = self.adapter.match(url, "GET")
        assert("handler.get_file_metadata"==endpoint)
        assert(self.uuid==args["uuid"])
    
    def test_file_metadata_POST(self):
        with pytest.raises(MethodNotAllowed):
            url = "/file/" + str(self.uuid) + "/metadata"
            self.adapter.match(url, "POST")

    def test_file_metadata_PUT(self):
        with pytest.raises(MethodNotAllowed):
            url = "/file/" + str(self.uuid) + "/metadata"
            self.adapter.match(url, "PUT")

    def test_file_metadata_DELETE(self):
        with pytest.raises(MethodNotAllowed):
            url = "/file/" + str(self.uuid) + "/metadata"
            self.adapter.match(url, "DELETE")
    
class TestSourceRoutes(object):
    @classmethod
    def setup_class(cls):
        cls.adapter = URL_MAP.bind("/")
    
    def test_sourceroot_GET(self):
        endpoint, args = self.adapter.match("/source/", "GET")
        assert("handler.get_sources"==endpoint)
    
    def test_sourceroot_POST(self):
        endpoint, args = self.adapter.match("/source/", "POST")
        assert("handler.add_source"==endpoint)

    def test_source_PUT(self):
        endpoint, args = self.adapter.match("/source/", "PUT")
        assert("handler.update_source"==endpoint)

    def test_source_DELETE(self):
        endpoint, args = self.adapter.match("/source/srcname", "DELETE")
        assert("handler.remove_source"==endpoint)
        assert("srcname"==args["name"])

class TestFilterRoutes(object):
    @classmethod
    def setup_class(cls):
        cls.adapter = URL_MAP.bind("/")

    def test_filterroot_GET(self):
        endpoint, args = self.adapter.match("/filter/", "GET")
        assert("handler.get_filters"==endpoint)
    
    def test_filterroot_POST(self):
        endpoint, args = self.adapter.match("/filter/", "POST")
        assert("handler.add_filter"==endpoint)
    
    def test_filter_GET(self):
        endpoint, args = self.adapter.match("/filter/FOO", "GET")
        assert("handler.get_filter"==endpoint)
        assert("FOO"==args["name"])

    def test_filter_PUT(self):
        endpoint, args = self.adapter.match("/filter/FOO", "PUT")
        assert("handler.update_filter"==endpoint)
        assert("FOO"==args["name"])
    
    def test_filter_DELETE(self):
        endpoint, args = self.adapter.match("/filter/FOO", "DELETE")
        assert("handler.remove_filter"==endpoint)
        assert("FOO"==args["name"])

class TestQueryRoutes(object):
    @classmethod
    def setup_class(cls):
        cls.adapter = URL_MAP.bind("/")
    
    def test_query_file_GET(self):
        with pytest.raises(MethodNotAllowed):
            self.adapter.match("/query/file", "GET")
    
    def test_query_file_POST(self):
        endpoint, args = self.adapter.match("/query/file", "POST")
        assert("handler.query_file"==endpoint)

    def test_query_file_PUT(self):
        with pytest.raises(MethodNotAllowed):
            self.adapter.match("/query/file", "PUT")

    def test_query_file_DELETE(self):
        with pytest.raises(MethodNotAllowed):
            self.adapter.match("/query/file", "DELETE")

    def test_query_attribute_GET(self):
        with pytest.raises(MethodNotAllowed):
            self.adapter.match("/query/attribute", "GET")
    
    def test_query_attribute_POST(self):
        endpoint, args = self.adapter.match("/query/attribute", "POST")
        assert("handler.query_attribute"==endpoint)

    def test_query_attribute_PUT(self):
        with pytest.raises(MethodNotAllowed):
            self.adapter.match("/query/attribute", "PUT")

    def test_query_attribute_DELETE(self):
        with pytest.raises(MethodNotAllowed):
            self.adapter.match("/query/attribute", "DELETE")

def test_get_handler():
    import baltrad.bdbserver.web.handler

    assert(baltrad.bdbserver.web.handler.get_file==get_handler("handler.get_file"))
