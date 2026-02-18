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

import pytest

from werkzeug import (
    test as wztest,
)

from baltrad.bdbserver.web import util

class TestJsonRequest(object):
    def test_float_nan_in_json(self):
        with pytest.raises(ValueError):        
            util.JsonResponse(float("nan"))
        
    def test_float_inf_in_json(self):
        with pytest.raises(ValueError):        
            util.JsonResponse(float("inf"))
    
    def test_float_neginf_in_json(self):
        with pytest.raises(ValueError):        
            util.JsonResponse(float("-inf"))

class TestHttpUnauthorized(object):
    def test_adds_www_authenticate_header(self):
        err =util.HttpUnauthorized(challenge="bdb-keyczar")
        env = wztest.EnvironBuilder().get_environ()
        response = err.get_response(env)
        expected = ["bdb-keyczar"]
        assert(expected==response.headers.get_all("www-authenticate"))
    
    def test_add_multiple_www_authenticate_headers(self):
        err =util.HttpUnauthorized(challenge=["bdb-keyczar", "bdb-noauth"])
        env = wztest.EnvironBuilder().get_environ()
        response = err.get_response(env)
        expected = ["bdb-keyczar", "bdb-noauth"]
        assert(expected==response.headers.get_all("www-authenticate"))
