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

from keyczar import keyczar

import mock

from nose.tools import eq_, ok_, raises

from werkzeug import (
    test as wztest,
    datastructures as wzdata
)

from baltrad.bdbserver import config
from baltrad.bdbserver.web import auth, util as webutil

from ..test_util import called_with, assert_called_once_with, called_once_with

class TestAuth(object):
    @mock.patch("pkg_resources.load_entry_point")
    def test_get_implementation(self, load_entry_point):
        load_entry_point.return_value = mock.sentinel.entry_point

        result = auth.Auth.get_implementation("mock")

        load_entry_point.assert_called_once_with(
            "baltrad.bdbserver",
            "baltrad.bdbserver.web.auth",
            "mock"
        )
        eq_(mock.sentinel.entry_point, result)

class TestKeyczarAuth(object):
    def setup(self):
        self.auth = auth.KeyczarAuth("/path/to/keys")
        self.verifier = mock.Mock(spec=keyczar.Verifier)
    
    @raises(ValueError)
    def test_construct_relative_keystore_root(self):
        auth.KeyczarAuth("relpath")
    
    @mock.patch("keyczar.keyczar.Verifier.Read")
    def test_add_key_abspath(self, load_verifier):
        load_verifier.return_value = self.verifier

        self.auth.add_key("keyname", "/path/to/key")
        
        assert_called_once_with(load_verifier, "/path/to/key")
        eq_(self.verifier, self.auth._verifiers["keyname"])

    @mock.patch("keyczar.keyczar.Verifier.Read")
    def test_add_key_relpath(self, load_verifier):
        load_verifier.return_value = self.verifier

        self.auth.add_key("keyname", "key")
        
        assert_called_once_with(load_verifier, "/path/to/keys/key")
        eq_(self.verifier, self.auth._verifiers["keyname"])
    
    def test_from_conf(self):
        # store original method, we are going to patch the class
        from_conf = auth.KeyczarAuth.from_conf
        self.auth = mock.Mock(spec=auth.KeyczarAuth)

        conf = config.Properties({
            "baltrad.bdb.server.auth.keyczar.keystore_root": "/keyroot",
            "baltrad.bdb.server.auth.keyczar.keys.keyname1": "/path/to/key",
            "baltrad.bdb.server.auth.keyczar.keys.keyname2": "relkey",
        })

        with mock.patch(
            "baltrad.bdbserver.web.auth.KeyczarAuth"
        ) as ctor:
            ctor.return_value = self.auth
            result = from_conf(conf)
            assert_called_once_with(ctor, "/keyroot")
            ok_(called_with(self.auth.add_key, "keyname1", "/path/to/key"))
            ok_(called_with(self.auth.add_key, "keyname2", "relkey"))
            eq_(result, self.auth)
    
    @raises(config.PropertyLookupError)
    def test_from_conf_missing_keystore_root(self):
        self.auth.from_conf(config.Properties({}))
    
    @mock.patch("baltrad.bdbserver.web.auth.create_signable_string")
    def test_authenticate(self, create_signable_string):
        req = mock.Mock(spec=webutil.Request)
        self.auth._verifiers["keyname"] = self.verifier
        self.verifier.Verify.return_value = True
        create_signable_string.return_value = "signable_string"

        ok_(self.auth.authenticate(req, "keyname:signature"))
        ok_(called_once_with(create_signable_string, req))
        ok_(called_once_with(
            self.verifier.Verify, "signable_string", "signature"
        ))
    
    @raises(auth.AuthError)
    def test_authenticate_missing_verifier(self):
        req = mock.Mock(spec=webutil.Request)
        self.auth.authenticate(req, "keyname:signature")
    
    @raises(auth.AuthError)
    def test_authenticate_invalid_credentials(self):
        req = mock.Mock(spec=webutil.Request)
        self.auth.authenticate(req, "foo")


class TestAuthMiddleware(object):
    def setup(self):
        self.app = mock.Mock()
        self.authmw = auth.AuthMiddleware(self.app)

    def test_authenticate(self):
        provider = mock.Mock(spec=auth.Auth)
        provider.authenticate.return_value = True
        self.authmw.add_provider("mock", provider)

        req = wztest.EnvironBuilder(
            headers=wzdata.Headers({
                "authorization": "bdb-mock mockcredentials",
            })
        ).get_request(webutil.Request)

        ok_(self.authmw.authenticate(req))
        ok_(called_once_with(provider.authenticate, req, "mockcredentials"))

    def test_authenticate_provider_failure(self):
        provider = mock.Mock(spec=auth.Auth)
        provider.authenticate.side_effect = Exception("should be caught")
        self.authmw.add_provider("mock", provider)

        req = wztest.EnvironBuilder(
            headers=wzdata.Headers({
                "authorization": "bdb-mock mockcredentials",
            })
        ).get_request(webutil.Request)


        ok_(not self.authmw.authenticate(req))
    
    def test_authenticate_invalid_credentials(self):
        req = wztest.EnvironBuilder(
            headers=wzdata.Headers({
                "authorization": "foobar"
            })
        ).get_request(webutil.Request)

        ok_(not self.authmw.authenticate(req))
    
    def test_authenticate_missing_provider(self):
        req = wztest.EnvironBuilder(
            headers=wzdata.Headers({
                "authorization": "bdb-noprovider mockcredentials"
            })
        ).get_request(webutil.Request)

        ok_(not self.authmw.authenticate(req))
    
    @mock.patch("baltrad.bdbserver.web.util.Request")
    def test_call(self, req_ctor):
        req_ctor.return_value = mock.sentinel.req
        self.authmw.authenticate = mock.Mock()
        self.authmw.authenticate.return_value = True

        self.authmw(mock.sentinel.env, mock.sentinel.start_response)
        
        self.app.assert_called_once_with(
            mock.sentinel.env, mock.sentinel.start_response
        )
        eq_(1, self.authmw.authenticate.call_count)
        self.authmw.authenticate.assert_called_once_with(mock.sentinel.req)
    
    @mock.patch("baltrad.bdbserver.web.util.HttpUnauthorized")
    @mock.patch("baltrad.bdbserver.web.util.Request")
    def test_call_auth_failure(self, req_ctor, unauthorized_ctor):
        req_ctor.return_value = mock.sentinel.req
        unauthorized = mock.Mock(spec=webutil.HttpUnauthorized)
        unauthorized_ctor.return_value = unauthorized
        self.authmw.authenticate = mock.Mock()
        self.authmw.authenticate.return_value = False

        self.authmw(mock.sentinel.env, mock.sentinel.start_response)
        unauthorized.assert_called_once_with(
            mock.sentinel.env, mock.sentinel.start_response
        )
    
    @mock.patch("baltrad.bdbserver.web.auth.Auth.get_implementation")
    def test_add_provider_from_conf(self, get_auth_implementation):
        provider_cls = get_auth_implementation.return_value
        provider_cls.from_conf.return_value = mock.sentinel.provider

        self.authmw.add_provider_from_conf("provider", mock.sentinel.conf)

        ok_(called_once_with(get_auth_implementation, "provider"))
        provider_cls.from_conf.assert_called_once_with(mock.sentinel.conf)
        eq_(mock.sentinel.provider, self.authmw._providers["provider"])

    def test_from_conf(self):
        # store original method, we are going to patch the class
        from_conf = auth.AuthMiddleware.from_conf
        self.authmw = mock.Mock(spec=auth.AuthMiddleware)
        conf = config.Properties({
            "baltrad.bdb.server.auth.providers": "foo, bar",
        })

        with mock.patch(
            "baltrad.bdbserver.web.auth.AuthMiddleware"
        ) as ctor:
            ctor.return_value = self.authmw
            result = from_conf(mock.sentinel.app, conf)
            ok_(called_once_with(ctor, mock.sentinel.app))
            ok_(called_with(self.authmw.add_provider_from_conf, "foo", conf))
            ok_(called_with(self.authmw.add_provider_from_conf, "bar", conf))
            eq_(result, self.authmw)
    
    def test_from_conf_no_providers(self):
        # store original method, we are going to patch the class
        from_conf = auth.AuthMiddleware.from_conf
        self.authmw = mock.Mock(spec=auth.AuthMiddleware)
        conf = config.Properties({
        })

        with mock.patch(
            "baltrad.bdbserver.web.auth.AuthMiddleware"
        ) as ctor:
            ctor.return_value = self.authmw
            result = from_conf(mock.sentinel.app, conf)
            ok_(called_once_with(ctor, mock.sentinel.app))
            ok_(called_with(self.authmw.add_provider_from_conf, "noauth", conf))
            eq_(result, self.authmw)
        
def test_get_credentials():
    req = wztest.EnvironBuilder(
        method="GET",
        headers=wzdata.Headers({
            "authorization": "bdb-keyczar keyname:signature",
        })
    ).get_request(webutil.Request)

    provider, credentials = auth.get_credentials(req)
    eq_("keyczar", provider)
    eq_("keyname:signature", credentials)

def test_get_credentials_missing_authorization():
    req = wztest.EnvironBuilder(
        method="GET",
    ).get_request(webutil.Request)

    eq_(("noauth", None), auth.get_credentials(req))

@raises(auth.AuthError)
def test_get_credentials_invalid_authorization1():
    req = wztest.EnvironBuilder(
        method="GET",
        headers=wzdata.Headers({
            "authorization": "foo",
        })
    ).get_request(webutil.Request)

    auth.get_credentials(req)

@raises(auth.AuthError)
def test_get_credentials_invalid_authorization2():
    req = wztest.EnvironBuilder(
        method="GET",
        headers=wzdata.Headers({
            "authorization": "foo bar"
        })
    ).get_request(webutil.Request)

    auth.get_credentials(req)

def test_create_signable_string():
    req = wztest.EnvironBuilder(
        method="GET",
        path="/path/to/resource",
        content_type="application/json",
        headers=wzdata.Headers({
            "date": "Tue, 10 Jan 2012 19:03:34 GMT",
            "content-md5": "a23ae94a35970e9689d30a56d954ef36",
            "authorization": "bdb-keyczar keyname:signature",
        })
    ).get_request(webutil.Request)
    expected = "\n".join((
        "GET",
        "/path/to/resource",
        "a23ae94a35970e9689d30a56d954ef36",
        "application/json",
        "Tue, 10 Jan 2012 19:03:34 GMT",
    ))
    eq_(expected, auth.create_signable_string(req))

def test_create_signable_string_path_with_query():
    req = wztest.EnvironBuilder(
        method="DELETE",
        path="/path/to/resource",
        query_string={"a": "b"},
    ).get_request(webutil.Request)
    expected = "\n".join((
        "DELETE",
        "/path/to/resource",
    ))
    eq_(expected, auth.create_signable_string(req))
