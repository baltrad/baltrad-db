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

from nose.tools import eq_, raises

from baltrad.bdb.web.routing import URL_MAP, get_handler

from werkzeug.exceptions import MethodNotAllowed

class TestFileRoutes(object):
    @classmethod
    def setup_class(cls):
        cls.adapter = URL_MAP.bind("/")
        cls.uuid = uuid.uuid4()
    
    @raises(MethodNotAllowed)
    def test_fileroot_GET(self):
        self.adapter.match("/file/", "GET")
    
    def test_fileroot_POST(self):
        endpoint, args = self.adapter.match("/file/", "POST")
        eq_("handler.add_file", endpoint)

    @raises(MethodNotAllowed)
    def test_fileroot_PUT(self):
        self.adapter.match("/file/", "PUT")
     
    @raises(MethodNotAllowed)
    def test_fileroot_DELETE(self):
        self.adapter.match("/file/", "DELETE")
    
    def test_file_GET(self):
        endpoint, args = self.adapter.match("/file/" + str(self.uuid), "GET")
        eq_("handler.get_file", endpoint)
        eq_(self.uuid, args["uuid"])
     
    @raises(MethodNotAllowed)
    def test_file_PUT(self):
        endpoint, args = self.adapter.match("/file/" + str(self.uuid), "PUT")

    @raises(MethodNotAllowed)
    def test_file_POST(self):
        endpoint, args = self.adapter.match("/file/" + str(self.uuid), "POST")
    
    
    def test_file_DELETE(self):
        endpoint, args = self.adapter.match("/file/" + str(self.uuid), "DELETE")
        eq_("handler.remove_file", endpoint)
        eq_(self.uuid, args["uuid"])
    
    def test_file_metadata_GET(self):
        url = "/file/" + str(self.uuid) + "/metadata"
        endpoint, args = self.adapter.match(url, "GET")
        eq_("handler.get_file_metadata", endpoint)
        eq_(self.uuid, args["uuid"])
    
    @raises(MethodNotAllowed)
    def test_file_metadata_POST(self):
        url = "/file/" + str(self.uuid) + "/metadata"
        self.adapter.match(url, "POST")

    @raises(MethodNotAllowed)
    def test_file_metadata_PUT(self):
        url = "/file/" + str(self.uuid) + "/metadata"
        self.adapter.match(url, "PUT")

    @raises(MethodNotAllowed)
    def test_file_metadata_DELETE(self):
        url = "/file/" + str(self.uuid) + "/metadata"
        self.adapter.match(url, "DELETE")
    
class TestSourceRoutes(object):
    @classmethod
    def setup_class(cls):
        cls.adapter = URL_MAP.bind("/")
    
    def test_sourceroot_GET(self):
        endpoint, args = self.adapter.match("/source/", "GET")
        eq_("handler.get_sources", endpoint)
    
    @raises(MethodNotAllowed)
    def test_sourceroot_POST(self):
        self.adapter.match("/source/", "POST")

    @raises(MethodNotAllowed)
    def test_sourceroot_PUT(self):
        self.adapter.match("/source/", "PUT")

    @raises(MethodNotAllowed)
    def test_sourceroot_DELETE(self):
        self.adapter.match("/source/", "DELETE")

class TestQueryRoutes(object):
    @classmethod
    def setup_class(cls):
        cls.adapter = URL_MAP.bind("/")
    
    @raises(MethodNotAllowed)
    def test_query_file_GET(self):
        self.adapter.match("/query/file", "GET")
    
    def test_query_file_POST(self):
        endpoint, args = self.adapter.match("/query/file", "POST")
        eq_("handler.query_file", endpoint)

    @raises(MethodNotAllowed)
    def test_query_file_PUT(self):
        self.adapter.match("/query/file", "PUT")

    @raises(MethodNotAllowed)
    def test_query_file_DELETE(self):
        self.adapter.match("/query/file", "DELETE")

    @raises(MethodNotAllowed)
    def test_query_attribute_GET(self):
        self.adapter.match("/query/attribute", "GET")
    
    def test_query_attribute_POST(self):
        endpoint, args = self.adapter.match("/query/attribute", "POST")
        eq_("handler.query_attribute", endpoint)

    @raises(MethodNotAllowed)
    def test_query_attribute_PUT(self):
        self.adapter.match("/query/attribute", "PUT")

    @raises(MethodNotAllowed)
    def test_query_attribute_DELETE(self):
        self.adapter.match("/query/attribute", "DELETE")

def test_get_handler():
    import baltrad.bdb.web.handler

    eq_(baltrad.bdb.web.handler.get_file, get_handler("handler.get_file"))
