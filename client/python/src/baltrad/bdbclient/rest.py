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

import httplib
import json
import urlparse

from baltrad.bdbcommon.oh5 import Source

from .db import Database, DatabaseError

class RestfulDatabase(Database):
    def __init__(self, server_url):
        self._server_url = urlparse.urlparse(server_url)
    
    def store(self, data):
        response = self._http_request(
            "POST", "/file/", data.read(),
            headers={
                "Content-Type": "application/x-hdf5",
            }
        )
        
        if response.status == httplib.CREATED:
            print "stored at", response.getheader("Location")
        else:
            raise DatabaseError("Unhandled response code: " + response.status)
    
    def get_file_entry(self, uuid):
        response = self._http_request(
            "GET", "/file/%s/metadata" % uuid
        )

        if response.status == httplib.OK:
            data = json.loads(response.read())
            return data["metadata"] #XXX: turn into metadata
        elif response.status == httplib.NOT_FOUND:
            return None
        else:
            raise DatabaseError("Unhandled response code: " + response.status)
    
    def get_file_content(self, uuid):
        response = self._http_request(
            "GET", "/file/%s" % uuid
        )
    
        if response.status == httplib.OK:
            return response
        elif response.status == httplib.NOT_FOUND:
            return None
        else:
            raise DatabaseError("Unhandled response code: " + response.status)
    
    def remove_file_entry(self, uuid):
        response = self._http_request(
            "DELETE", "/file/%s" % uuid
        )
    
        if response.status == httplib.OK:
            return True
        elif response.status == httplib.NOT_FOUND:
            return False
        else:
            raise DatabaseError("Unhandled response code: " + response.status)
    
    def get_sources(self):
        response = self._http_request(
            "GET", "/source/"
        )

        if response.status == httplib.OK:
            data = json.loads(response.read())
            return [Source(default=src) for src in data["sources"]]
        else:
            raise DatabaseError("Unhandled response code: " + response.status)
    
    def add_source(self, source):
        response = self._http_request(
            "POST", "/source/", json.dumps({
                "source" : {
                    "name": source.name,
                    "values": dict(source),
                }
            })
        )

        if response.status == httplib.CREATED:
            pass
        elif response.status == httplib.CONFLICT:
            raise DatabaseError("duplicate entry")
        else:
            raise DatabaseError("Unhandled response code: %s" % response.status)

    def _http_request(self, method, path, data="", headers={}):
        conn = httplib.HTTPConnection(
            self._server_url.hostname,
            self._server_url.port
        )
        conn.request(method, path, data, headers)
        return conn.getresponse()
