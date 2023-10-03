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

import os

from baltradcrypto.crypto import keyczarcrypto as keyczar

from nose.tools import eq_, ok_, raises

from werkzeug import (
    test as wztest,
    datastructures as wzdata
)

from baltrad.bdbserver.web import auth

class TestKeyczarAuth(object):
    def setup(self):
        self.keystore = os.path.join(os.path.dirname(__file__), "fixture")
        self.key = os.path.join(self.keystore, "testkey")
        self.auth = auth.KeyczarAuth(self.keystore)

    def add_key(self):
        self.auth.add_key("testkey", "testkey")
    
    @raises(Exception)
    def test_add_nx_key(self):
        self.auth.add_key("testkey", "/path/to/nxkey")
    
    @raises(Exception)
    def test_add_invalid_key(self):
        self.auth.add_key("testkey", "invalidkey")
    
    def test_authenticate(self):
        self.auth.add_key("testkey", "testkey")
        req = wztest.EnvironBuilder(
            method="GET",
            path="/path/to/resource",
            content_type="application/json",
            headers=wzdata.Headers({
                "date": "Tue, 10 Jan 2012 19:03:34 GMT",
                "content-md5": "a23ae94a35970e9689d30a56d954ef36",
                "authorization": "".join((
                    "bdb-keyczar testkey:"
                    "AHjESBEwLAIUfrk7ItBEHYvHgLex9BtOTs"
                    "2yEGcCFFbBhvU891VRQISAPgUnHOuY1VQ3"
                ))
            })
        )

        provider, credentials = auth.get_credentials(req)
        eq_("keyczar", provider)
        ok_(self.auth.authenticate(req, credentials))
    
    def test_authenticate_invalid_signature(self):
        self.auth.add_key("testkey", "testkey")
        req = wztest.EnvironBuilder(
            method="GET",
            path="/path/to/resource",
            content_type="application/json",
            headers=wzdata.Headers({
                "date": "Tue, 10 Jan 2012 19:03:34 GMT",
                "content-md5": "a23ae94a35970e9689d30a56d954ef36",
                "authorization": "".join((
                    "bdb-keyczar testkey:"
                    "123ESBEwLAIUfrk7ItBEHYvHgLex9BtOTs"
                    "2yEGcCFFbBhvU891VRQISAPgUnHOuY1VQ3"
                ))
            })
        )

        provider, credentials = auth.get_credentials(req)
        eq_("keyczar", provider)
        eq_(False, self.auth.authenticate(req, credentials))
