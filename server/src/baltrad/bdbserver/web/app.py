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

import urlparse

from werkzeug import serving, exceptions as wzexc

from baltrad.bdbserver import backend
from baltrad.bdbserver.web import auth, routing, util as webutil

import logging
logger = logging.getLogger("baltard.bdbserver.app")

class Application(object):
    def __init__(self, backend):
        self.backend = backend
        self.url_map = routing.URL_MAP
    
    def dispatch_request(self, request):
        adapter = self.url_map.bind_to_environ(request.environ)
        ctx = webutil.RequestContext(request, self.backend)
        ctx.enable_remove_all_files = self.enable_remove_all_files
        try:
            endpoint, values = adapter.match()
            handler = routing.get_handler(endpoint)
            return handler(ctx, **values)
        except wzexc.HTTPException, e:
            return e
    
    def __call__(self, env, start_response):
        request = webutil.Request(env)
        response = self.dispatch_request(request)
        return response(env, start_response)
    
    @classmethod
    def from_conf(cls, backend, conf):
        """create instance from configuration.

        :param conf: a :class:`~.config.Properties` instance to configure
                     from
        """
        result = Application(backend)
        result.enable_remove_all_files = conf.get_boolean(
            "baltrad.bdb.server.enable_remove_all_files", False
        )
        return result

def from_conf(conf):
    """create the entire WSGI application from conf

    this will wrap the application with necessary middleware
    """
    be = backend.from_conf(conf)
    if not be.is_operational():
        raise SystemExit("backend is not operational")
    app = Application.from_conf(be, conf)
    authmw = auth.AuthMiddleware.from_conf(app, conf)
    return authmw
 
def serve(uri, app):
    """serve the application using werkzeug
    """

    uri = urlparse.urlparse(uri)
    host = uri.hostname
    port = uri.port or 80

    serving.run_simple(host, port, app) 
