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

import mock

from nose.tools import eq_, ok_, raises

from baltrad.bdbserver import backend, config

from .test_util import called_once_with

@mock.patch("baltrad.bdbserver.backend.Backend.get_implementation")
def test_from_conf(get_backend_impl):
    backend_cls = mock.Mock()
    backend_cls.create_from_config.return_value = mock.sentinel.backend
    get_backend_impl.return_value = backend_cls
    conf = config.Properties({
        "baltrad.bdb.server.backend.type": "mock"
    })

    eq_(mock.sentinel.backend, backend.from_conf(conf))
    ok_(called_once_with(get_backend_impl, "mock"))
    ok_(called_once_with(backend_cls.create_from_config, conf))

@raises(config.Error)
@mock.patch("baltrad.bdbserver.backend.Backend.get_implementation")
def test_from_conf_invalid_implementation(get_backend_impl):
    get_backend_impl.side_effect = LookupError
    conf = config.Properties({
        "baltrad.bdb.server.backend.type": "mock"
    })

    backend.from_conf(conf)
