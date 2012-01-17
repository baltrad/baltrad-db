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

from keyczar import errors as kzerrors

from nose.tools import eq_, ok_, raises

from baltrad.bdbclient import rest

class TestKeyczarAuth(object):
    @raises(kzerrors.KeyczarError)
    def test_ctor_nx_key(self):
        rest.KeyczarAuth("/path/to/nxkey")
    
    @raises(kzerrors.KeyczarError)
    def test_ctor_invalid_key(self):
        rest.KeyczarAuth(
            os.path.join(
                os.path.dirname(__file__),
                "fixture",
                "invalidkey"
            )
        )
    
    def test_sign(self):
        auth = rest.KeyczarAuth(
            os.path.join(
                os.path.dirname(__file__),
                "fixture",
                "testkey"
            )
        )

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
        credentials = req.headers["authorization"]
        ok_(credentials.startswith("bdb-keyczar testkey:AH"))
