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

import sys
patch_httpexception="httplib.HTTPConnection"
if sys.version_info < (3,):
    import httplib as httplibclient
    import urlparse
    patch_httpexception="httplib.HTTPConnection"    
else:
    from http import client as httplibclient
    from urllib.parse import urlparse
    patch_httpexception="http.client.HTTPConnection"    
    
import pytest
import mock

from baltrad.bdbclient import db, rest


def test_create_signable_string():
    expected = "\n".join((
        "GET",
        "/path/to/resource",
        "a23ae94a35970e9689d30a56d954ef36",
        "application/json",
        "Tue, 10 Jan 2012 19:03:34 GMT",
    ))

    req = rest.Request(
        "GET",
        "/path/to/resource",
        headers={
            "content-type": "application/json",
            "date": "Tue, 10 Jan 2012 19:03:34 GMT",
            "content-md5": "a23ae94a35970e9689d30a56d954ef36",
            "some-header": "some-value",
        }
    )
    result = rest.create_signable_string(req)

    assert(expected == result)

class TestKeyczarAuth(object):
    @mock.patch("baltradcrypto.crypto.keyczarcrypto.keyczar_signer.read")
    def test_ctor(self, signer_read):
        signer_read.return_value = mock.sentinel.signer

        result = rest.KeyczarAuth("/path/to/key", key_name="keyname")
        assert(result._key_name == "keyname")
        assert(result._signer == mock.sentinel.signer)
        signer_read.assert_called_with("/path/to/key")
    
    @mock.patch("baltradcrypto.crypto.keyczarcrypto.keyczar_signer.read")
    def test_ctor_default_keyname(self, signer_read):
        signer_read.return_value = mock.sentinel.signer

        result = rest.KeyczarAuth("/path/to/key")
        assert(result._key_name == "key")
        assert(result._signer == mock.sentinel.signer)
        signer_read.assert_called_with("/path/to/key")
    
    @mock.patch("baltrad.bdbclient.rest.create_signable_string")
    @mock.patch("baltradcrypto.crypto.keyczarcrypto.keyczar_signer.read")
    def test_sign(self, signer_read, create_signable):
        signer = mock.Mock()
        create_signable.return_value = "signable"
        signer.sign.return_value = "signature"
        signer_read.return_value = signer
        auth = rest.KeyczarAuth("/path/to/key")
        req = rest.Request(
            "GET",
            "/path/to/resource",
            headers={
                "content-type": "application/json",
                "date": "Tue, 10 Jan 2012 19:03:34 GMT",
                "content-md5": "a23ae94a35970e9689d30a56d954ef36",
            }
        )

        auth.add_credentials(req)
        assert(req.headers["authorization"] == "bdb-keyczar key:signature")
        create_signable.assert_called_with(req)
        signer.sign.assert_called_with("signable")

class TestRestfulFileResult(object):
    def setup_method(self, method):
        self.db = mock.Mock(spec=rest.RestfulDatabase)
        self.result = rest.RestfulFileResult(
            self.db,
            rows=[
                {"uuid": "00000000-0000-0000-0004-000000000001"},
                {"uuid": "00000000-0000-0000-0004-000000000002"},
            ]
        )
    
    def test_next(self):
        assert(self.result.next())
        assert(self.result.next())
        assert(not self.result.next())
        assert(not self.result.next())
    
    def test_size(self):
        assert(self.result.size() == 2)
    
    def test_get_file_entry(self):
        self.result.next()
        self.db.get_file_entry.return_value = mock.sentinel.entry1
        assert(self.result.get_file_entry() == mock.sentinel.entry1)
        self.db.get_file_entry.assert_called_once_with(
            "00000000-0000-0000-0004-000000000001"
        )
        self.result.next()
        self.db.get_file_entry.reset_mock()
        self.db.get_file_entry.return_value = mock.sentinel.entry2
        assert(self.result.get_file_entry() == mock.sentinel.entry2)
        self.db.get_file_entry.assert_called_once_with(
            "00000000-0000-0000-0004-000000000002"
        )
    
    def test_get_file_entry_before_next(self):
        with pytest.raises(LookupError):
            self.result.get_file_entry()
        
    def test_get_file_entry_after_exhaustion(self):
        self.result.next()
        self.result.next()
        self.result.next()
        with pytest.raises(LookupError):
            self.result.get_file_entry()
    
    def test_get_uuid(self):
        self.result.next()
        assert(self.result.get_uuid() == "00000000-0000-0000-0004-000000000001")
        self.result.next()
        assert(self.result.get_uuid() == "00000000-0000-0000-0004-000000000002")
    
    def test_get_uuid_before_next(self):
        with pytest.raises(LookupError):
            self.result.get_uuid()
    
    def test_get_uuid_after_exhaustion(self):
        self.result.next()
        self.result.next()
        self.result.next()
        with pytest.raises(LookupError):
            self.result.get_uuid()
        

class TestRestfulAttributeResult(object):
    def setup_method(self):
        self.result = rest.RestfulAttributeResult(
            rows=[
                {"uuid": "00000000-0000-0000-0004-000000000001"},
                {"uuid": "00000000-0000-0000-0004-000000000002"},
            ]
        )
    
    def test_next(self):
        assert(self.result.next())
        assert(self.result.next())
        assert(not self.result.next())
        assert(not self.result.next())
    
    def test_size(self):
        assert(self.result.size() == 2)
    
    def test_get(self):
        self.result.next()
        assert(self.result.get("uuid") == "00000000-0000-0000-0004-000000000001")
        self.result.next()
        assert(self.result.get("uuid") == "00000000-0000-0000-0004-000000000002")
    
    def test_get_invalid_key(self):
        self.result.next()
        with pytest.raises(LookupError):
            self.result.get("invalid key")
    
    def test_get_before_next(self):
        with pytest.raises(LookupError):
            self.result.get("uuid")
    
    def test_get_after_exhaustion(self):
        self.result.next()
        self.result.next()
        self.result.next()
        with pytest.raises(LookupError):
            self.result.get("uuid")

class TestRestfulDatabase(object):
    def setup_method(self):
        self.auth = mock.Mock(spec=rest.Auth)
        self.db = rest.RestfulDatabase("http://www.example.com", self.auth)
        self.db.execute_request = mock.Mock()
    
    @mock.patch("baltrad.bdbclient.rest.Request")
    def test_execute_file_query(self, request_ctor):
        response = mock.Mock()
        response.status = httplibclient.OK
        response.read.return_value = ('{"rows": ['
            '{"uuid": "00000000-0000-0000-0004-000000000001"},'
            '{"uuid": "00000000-0000-0000-0004-000000000002"}'
        ']}')
        request_ctor.return_value = mock.sentinel.request
        self.db.execute_request.return_value = response
        query = mock.Mock(spec=db.FileQuery)
        query.to_json.return_value = mock.sentinel.query_json

        result = self.db.execute_file_query(query)
        request_ctor.assert_called_once_with(
            "POST", "/query/file",
            data=mock.sentinel.query_json,
            headers={
                "content-type": "application/json"
            }
        )
        self.db.execute_request.assert_called_once_with(mock.sentinel.request)
        assert(isinstance(result, rest.RestfulFileResult))
        assert(result.size() == 2)

    @mock.patch("baltrad.bdbclient.rest.Request")
    def test_execute_attribute_query(self, request_ctor):
        response = mock.Mock()
        response.status = httplibclient.OK
        response.read.return_value = ('{"rows": ['
            '{"uuid": "00000000-0000-0000-0004-000000000001"},'
            '{"uuid": "00000000-0000-0000-0004-000000000002"}'
        ']}')
        request_ctor.return_value = mock.sentinel.request
        self.db.execute_request.return_value = response
        query = mock.Mock(spec=db.AttributeQuery)
        query.to_json.return_value = mock.sentinel.query_json

        result = self.db.execute_attribute_query(query)
        request_ctor.assert_called_once_with(
            "POST", "/query/attribute",
            data=mock.sentinel.query_json,
            headers={
                "content-type": "application/json"
            }
        )
        self.db.execute_request.assert_called_once_with(mock.sentinel.request)
        assert(isinstance(result, rest.RestfulAttributeResult))
        assert(result.size() == 2)
    
    @mock.patch(patch_httpexception) #"httplib.HTTPConnection")
    def test_execute_request(self, conn_ctor):
        self.db = rest.RestfulDatabase("http://www.example.com", self.auth)
        req = mock.Mock(spec=rest.Request)
        req.method = mock.sentinel.method
        req.path = mock.sentinel.path
        req.data = mock.sentinel.data
        req.headers = mock.sentinel.headers
        conn = mock.Mock()
        conn.getresponse.return_value = mock.sentinel.response
        conn_ctor.return_value = conn

        assert(self.db.execute_request(req) == mock.sentinel.response)

        conn_ctor.assert_called_with("www.example.com", None)
        self.auth.add_credentials.assert_called_with(req)
        conn.request.assert_called_with(
            mock.sentinel.method,
            mock.sentinel.path,
            mock.sentinel.data,
            mock.sentinel.headers
        )
        conn.getresponse.assert_called()
