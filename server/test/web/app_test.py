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
from baltrad.bdbserver.web import app

from ..test_util import called_once_with

class TestApplication(object):
    def setup(self):
        self.backend = mock.Mock(spec=backend.Backend)
        self.app = app.Application(self.backend)

    def test_from_conf(self):
        # store original method, we are going to patch the class
        from_conf = app.Application.from_conf
        self.app = mock.Mock(spec=app.Application)

        conf = config.Properties({
            "baltrad.bdb.server.enable_remove_all_files": True
        })

        with mock.patch("baltrad.bdbserver.web.app.Application") as ctor:
            ctor.return_value = self.app
            result = from_conf(self.backend, conf)
            ctor.assert_called_once_with(self.backend)
            eq_(True, result.enable_remove_all_files)

    def test_from_conf_defaults(self):
        # store original method, we are going to patch the class
        from_conf = app.Application.from_conf
        self.app = mock.Mock(spec=app.Application)

        conf = config.Properties({
        })

        with mock.patch("baltrad.bdbserver.web.app.Application") as ctor:
            ctor.return_value = self.app
            result = from_conf(self.backend, conf)
            ctor.assert_called_once_with(self.backend)
            eq_(False, result.enable_remove_all_files)

@mock.patch("baltrad.bdbserver.web.auth.AuthMiddleware.from_conf")
@mock.patch("baltrad.bdbserver.web.app.Application.from_conf")
@mock.patch("baltrad.bdbserver.backend.from_conf")
def test_from_conf(backend_ctor, app_ctor, authmw_ctor):
    mock_backend = mock.Mock(spec=backend.Backend)
    mock_backend.is_operational.return_value = True
    backend_ctor.return_value = mock_backend
    app_ctor.return_value = mock.sentinel.app
    authmw_ctor.return_value = mock.sentinel.authmw

    eq_(mock.sentinel.authmw, app.from_conf(mock.sentinel.conf))

    ok_(called_once_with(backend_ctor, mock.sentinel.conf))
    ok_(called_once_with(mock_backend.is_operational))
    ok_(called_once_with(app_ctor, mock_backend, mock.sentinel.conf))
    ok_(called_once_with(authmw_ctor, mock.sentinel.app, mock.sentinel.conf))

@raises(SystemExit)
@mock.patch("baltrad.bdbserver.backend.from_conf")
def test_from_conf_backend_not_operational(backend_ctor):
    mock_backend = mock.Mock(spec=backend.Backend)
    mock_backend.is_operational.return_value = False
    backend_ctor.return_value = mock_backend
    
    app.from_conf(mock.sentinel.conf)
    ok_(called_once_with(backend_ctor, mock.sentinel.conf))
    ok_(called_once_with(mock_backend.is_operational))

