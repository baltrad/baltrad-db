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
import os
import socket
import urlparse

from baltrad.bdbcommon.oh5 import (
    Attribute,
    Dataset,
    Group,
    Metadata,
    Source,
)

from .db import Database, DatabaseError, FileEntry

class RestfulFileEntry(FileEntry):
    def __init__(self, metadata):
        self._metadata = metadata
    
    @property
    def uuid(self):
        return self._metadata.bdb_uuid
    
    @property
    def metadata(self):
        return self._metadata

def metadata_from_json_repr(json_repr):
    metadata = Metadata()
    for node_repr in json_repr:
        parent_path, node_name = os.path.split(node_repr["path"])
        node = node_from_json_repr(node_name, node_repr)
        metadata.add_node(parent_path, node)
    return metadata

def node_from_json_repr(name, json_repr):
    type_ = json_repr["type"]
    if type_ == "attribute":
        return Attribute(name, json_repr["value"])
    elif type_ == "group":
        return Group(name)
    elif type_ == "dataset":
        return Dataset(name)
    else:
        raise RuntimeError("unhandled node type: %s" % type_)

class RestfulDatabase(Database):
    def __init__(self, server_url):
        self._server_url_str = server_url
        self._server_url = urlparse.urlparse(server_url)
    
    def store(self, data):
        response = self._http_request(
            "POST", "/file/", data.read(),
            headers={
                "Content-Type": "application/x-hdf5",
            }
        )
        
        if response.status == httplib.CREATED:
            data = json.loads(response.read())
            metadata = metadata_from_json_repr(data["metadata"])
            return RestfulFileEntry(metadata)
        else:
            raise DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def get_file_entry(self, uuid):
        response = self._http_request(
            "GET", "/file/%s/metadata" % uuid
        )

        if response.status == httplib.OK:
            data = json.loads(response.read())
            metadata = metadata_from_json_repr(data["metadata"])
            return RestfulFileEntry(metadata)
        elif response.status == httplib.NOT_FOUND:
            return None
        else:
            raise DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def get_file_content(self, uuid):
        response = self._http_request(
            "GET", "/file/%s" % uuid
        )
    
        if response.status == httplib.OK:
            return response
        elif response.status == httplib.NOT_FOUND:
            return None
        else:
            raise DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def remove_file_entry(self, uuid):
        response = self._http_request(
            "DELETE", "/file/%s" % uuid
        )
    
        if response.status == httplib.NO_CONTENT:
            return True
        elif response.status == httplib.NOT_FOUND:
            return False
        else:
            raise DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def get_sources(self):
        response = self._http_request(
            "GET", "/source/"
        )

        if response.status == httplib.OK:
            data = json.loads(response.read())
            return [
                Source(src["name"], values=src["values"])
                    for src in data["sources"]
            ]
        else:
            raise DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
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
            raise DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def update_source(self, name, source):
        response = self._http_request(
            "PUT", "/source/%s" % name, json.dumps({
                "source": {
                    "name": source.name,
                    "values": dict(source),
                }
            })
        )

        if response.status == httplib.NO_CONTENT:
            return
        elif response.status == httplib.NOT_FOUND:
            raise LookupError("source '%s' not found" % name)
        elif response.status == httplib.CONFLICT:
            raise DatabaseError("source '%s' already exists" % source.name)
        else:
            raise DatabaseError(
                "Unhandled response code: %s" % response.status
            )

    def remove_source(self, name):
        response = self._http_request(
            "DELETE", "/source/%s" % name
        )

        if response.status == httplib.NO_CONTENT:
            return True
        elif response.status == httplib.NOT_FOUND:
            return False
        elif response.status == httplib.CONFLICT:
            raise DatabaseError(
                "couldn't remove source, (it might be associated with files)"
            )
        else:
            raise DatabaseError(
                "Unhandled response code: %s" % response.status
            )

    def _http_request(self, method, path, data="", headers={}):
        conn = httplib.HTTPConnection(
            self._server_url.hostname,
            self._server_url.port
        )
        try:
            conn.request(method, path, data, headers)
        except socket.error:
            raise DatabaseError(
                "Could not send request to %s" % self._server_url_str
            )
        return conn.getresponse()
