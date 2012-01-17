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

from baltrad.bdbserver import backend
from baltrad.bdbserver.web import app

from ..test_util import called_once_with

@mock.patch("baltrad.bdbserver.web.auth.AuthMiddleware.from_conf")
@mock.patch("baltrad.bdbserver.web.app.Application.from_conf")
@mock.patch("baltrad.bdbserver.backend.from_conf")
def test_from_conf(backend_ctor, app_ctor, authmw_ctor):
    mock_backend = mock.Mock(spec=backend.Backend)
    mock_backend.is_operational.return_value = True
    mock_app = mock.Sentinel()
    mock_authmw = mock.Sentinel()
    mock_conf = mock.Sentinel()
    backend_ctor.return_value = mock_backend
    app_ctor.return_value = mock_app
    authmw_ctor.return_value = mock_authmw

    eq_(mock_authmw, app.from_conf(mock_conf))

    ok_(called_once_with(backend_ctor, mock_conf))
    ok_(called_once_with(mock_backend.is_operational))
    ok_(called_once_with(app_ctor, mock_backend, mock_conf))
    ok_(called_once_with(authmw_ctor, mock_app, mock_conf))

@raises(SystemExit)
@mock.patch("baltrad.bdbserver.backend.from_conf")
def test_from_conf_backend_not_operational(backend_ctor):
    mock_backend = mock.Mock(spec=backend.Backend)
    mock_backend.is_operational.return_value = False
    backend_ctor.return_value = mock_backend
    mock_conf = mock.Sentinel()
    
    app.from_conf(mock_conf)
    ok_(called_once_with(backend_ctor, mock_conf))
    ok_(called_once_with(mock_backend.is_operational))

