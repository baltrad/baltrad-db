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

from nose.tools import eq_, ok_, raises

from werkzeug import (
    test as wztest,
)

from baltrad.bdbserver.web import util

class TestJsonRequest(object):
    @raises(ValueError)
    def test_float_nan_in_json(self):
        util.JsonResponse(float("nan"))
        
    @raises(ValueError)
    def test_float_inf_in_json(self):
        util.JsonResponse(float("inf"))
    
    @raises(ValueError)
    def test_float_neginf_in_json(self):
        util.JsonResponse(float("-inf"))

class TestHttpUnauthorized(object):
    def test_adds_www_authenticate_header(self):
        err =util.HttpUnauthorized(challenge="bdb-keyczar")
        env = wztest.EnvironBuilder().get_environ()
        response = err.get_response(env)
        expected = ["bdb-keyczar"]
        eq_(expected, response.headers.get_all("www-authenticate"))
    
    def test_add_multiple_www_authenticate_headers(self):
        err =util.HttpUnauthorized(challenge=["bdb-keyczar", "bdb-noauth"])
        env = wztest.EnvironBuilder().get_environ()
        response = err.get_response(env)
        expected = ["bdb-keyczar", "bdb-noauth"]
        eq_(expected, response.headers.get_all("www-authenticate"))
