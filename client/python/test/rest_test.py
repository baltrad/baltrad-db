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

import httplib

from nose.tools import eq_, ok_, raises
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

    eq_(expected, result)

class TestKeyczarAuth(object):
    @mock.patch("keyczar.keyczar.Signer.Read")
    def test_ctor(self, signer_read):
        signer_read.return_value = mock.sentinel.signer

        result = rest.KeyczarAuth("/path/to/key", key_name="keyname")
        eq_("keyname", result._key_name)
        eq_(mock.sentinel.signer, result._signer)
        signer_read.assert_called_with("/path/to/key")
    
    @mock.patch("keyczar.keyczar.Signer.Read")
    def test_ctor_default_keyname(self, signer_read):
        signer_read.return_value = mock.sentinel.signer

        result = rest.KeyczarAuth("/path/to/key")
        eq_("key", result._key_name)
        eq_(mock.sentinel.signer, result._signer)
        signer_read.assert_called_with("/path/to/key")
    
    @mock.patch("baltrad.bdbclient.rest.create_signable_string")
    @mock.patch("keyczar.keyczar.Signer.Read")
    def test_sign(self, signer_read, create_signable):
        signer = mock.Mock()
        create_signable.return_value = "signable"
        signer.Sign.return_value = "signature"
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
        eq_("bdb-keyczar key:signature", req.headers["authorization"])
        create_signable.assert_called_with(req)
        signer.Sign.assert_called_with("signable")

class TestRestfulFileResult(object):
    def setup(self):
        self.db = mock.Mock(spec=rest.RestfulDatabase)
        self.result = rest.RestfulFileResult(
            self.db,
            rows=[
                {"uuid": "00000000-0000-0000-0004-000000000001"},
                {"uuid": "00000000-0000-0000-0004-000000000002"},
            ]
        )
    
    def test_next(self):
        ok_(self.result.next())
        ok_(self.result.next())
        ok_(not self.result.next())
        ok_(not self.result.next())
    
    def test_size(self):
        eq_(2, self.result.size())

    
    def test_get_file_entry(self):
        self.result.next()
        self.db.get_file_entry.return_value = mock.sentinel.entry1
        eq_(mock.sentinel.entry1, self.result.get_file_entry())
        self.db.get_file_entry.assert_called_once_with(
            "00000000-0000-0000-0004-000000000001"
        )
        self.result.next()
        self.db.get_file_entry.reset_mock()
        self.db.get_file_entry.return_value = mock.sentinel.entry2
        eq_(mock.sentinel.entry2, self.result.get_file_entry())
        self.db.get_file_entry.assert_called_once_with(
            "00000000-0000-0000-0004-000000000002"
        )
    
    @raises(LookupError)
    def test_get_file_entry_before_next(self):
        self.result.get_file_entry()
        
    @raises(LookupError)
    def test_get_file_entry_after_exhaustion(self):
        self.result.next()
        self.result.next()
        self.result.next()
        self.result.get_file_entry()
    
    def test_get_uuid(self):
        self.result.next()
        eq_("00000000-0000-0000-0004-000000000001", self.result.get_uuid())
        self.result.next()
        eq_("00000000-0000-0000-0004-000000000002", self.result.get_uuid())
    
    @raises(LookupError)
    def test_get_uuid_before_next(self):
        self.result.get_uuid()
    
    @raises(LookupError)
    def test_get_uuid_after_exhaustion(self):
        self.result.next()
        self.result.next()
        self.result.next()
        self.result.get_uuid()
        
class TestRestfulDatabase(object):
    def setup(self):
        self.auth = mock.Mock(spec=rest.Auth)
        self.db = rest.RestfulDatabase("http://www.example.com", self.auth)
        self.db.execute_request = mock.Mock()
    
    @mock.patch("baltrad.bdbclient.rest.Request")
    def test_execute_file_query(self, request_ctor):
        response = mock.Mock()
        response.status = httplib.OK
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
        ok_(isinstance(result, rest.RestfulFileResult))
        eq_(2, result.size())
    
    @mock.patch("httplib.HTTPConnection")
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

        eq_(mock.sentinel.response, self.db.execute_request(req))

        conn_ctor.assert_called_with("www.example.com", None)
        self.auth.add_credentials.assert_called_with(req)
        conn.request.assert_called_with(
            mock.sentinel.method,
            mock.sentinel.path,
            mock.sentinel.data,
            mock.sentinel.headers
        )
        conn.getresponse.assert_called()
