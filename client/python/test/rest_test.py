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

from baltrad.bdbclient import rest

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
        signer = mock.Sentinel()
        signer_read.return_value = signer

        result = rest.KeyczarAuth("/path/to/key", key_name="keyname")
        eq_("keyname", result._key_name)
        eq_(signer, result._signer)
        signer_read.assert_called_with("/path/to/key")
    
    @mock.patch("keyczar.keyczar.Signer.Read")
    def test_ctor_default_keyname(self, signer_read):
        signer = mock.Sentinel()
        signer_read.return_value = signer

        result = rest.KeyczarAuth("/path/to/key")
        eq_("key", result._key_name)
        eq_(signer, result._signer)
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

class TestRestfulDatabase(object):
    def setup(self):
        self.auth = mock.Mock(spec=rest.Auth)
        self.db = rest.RestfulDatabase("http://www.example.com", self.auth)
    
    @mock.patch("httplib.HTTPConnection")
    def test__execute(self, conn_ctor):
        req = mock.Mock(spec=rest.Request)
        req.method = mock.Sentinel()
        req.path = mock.Sentinel()
        req.data = mock.Sentinel()
        req.headers = mock.Sentinel()
        response = mock.Sentinel()
        #conn = mock.Mock(spec=httplib.HTTPConnection)
        conn = mock.Mock()
        conn.getresponse.return_value = response
        conn_ctor.return_value = conn

        eq_(response, self.db._execute(req))

        conn_ctor.assert_called_with("www.example.com", None)
        self.auth.add_credentials.assert_called_with(req)
        conn.request.assert_called_with(req.method, req.path, req.data, req.headers)
        conn.getresponse.assert_called()
