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

from werkzeug.serving import run_simple
from werkzeug.exceptions import HTTPException

from . import routing
from .util import Request, RequestContext

class Application(object):
    def __init__(self, backend):
        self.backend = backend
        self.url_map = routing.URL_MAP
    
    def dispatch_request(self, request):
        adapter = self.url_map.bind_to_environ(request.environ)
        ctx = RequestContext(request, self.backend)
        try:
            endpoint, values = adapter.match()
            handler = routing.get_handler(endpoint)
            return handler(ctx, **values)
        except HTTPException, e:
            return e
    
    def __call__(self, env, start_response):
        request = Request(env)
        response = self.dispatch_request(request)
        return response(env, start_response)

def serve(app, config):
    host = config["server"].get("host")
    port = config["server"].getint("port")
    use_reloader = config["server"].getboolean("use_reloader")

    run_simple(host, port, app, use_reloader=use_reloader)
