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

import abc
import sys
if sys.version_info < (3,):
    import httplib as httplibclient
    import urlparse
else:
    from http import client as httplibclient
    import urllib.parse as urlparse
    #from urllib.parse import urlparse

import json
import os
import socket

from baltradcrypto.crypto import keyczarcrypto

from baltrad.bdbcommon import oh5
from baltrad.bdbclient import db

class Request(object):
    def __init__(self, method, path, data=None, headers={}):
        self.method = method
        self.path = path
        self.data = data
        self.headers = headers

class RestfulFileEntry(db.FileEntry):
    def __init__(self, metadata):
        self._metadata = metadata
    
    @property
    def uuid(self):
        return self._metadata.bdb_uuid
    
    @property
    def metadata(self):
        return self._metadata

def metadata_from_json_repr(json_repr):
    metadata = oh5.Metadata()
    for node_repr in json_repr:
        parent_path, node_name = os.path.split(node_repr["path"])
        node = node_from_json_repr(node_name, node_repr)
        metadata.add_node(parent_path, node)
    return metadata

def node_from_json_repr(name, json_repr):
    type_ = json_repr["type"]
    if type_ == "attribute":
        return oh5.Attribute(name, json_repr["value"])
    elif type_ == "group":
        return oh5.Group(name)
    elif type_ == "dataset":
        return oh5.Dataset(name)
    else:
        raise RuntimeError("unhandled node type: %s" % type_)

class RestfulDatabase(db.Database):
    """Access database over the RESTful interface
    """
    def __init__(self, server_url, auth):
        self._server_url_str = server_url
        self._server_url = urlparse.urlparse(server_url)
        self._auth = auth
    
    def store(self, data):
        request = Request(
            "POST", "/file/", data.read(),
            headers={
                "Content-Type": "application/x-hdf5",
            }
        )

        response = self.execute_request(request)
        
        if response.status == httplibclient.CREATED:
            data = json.loads(response.read())
            metadata = metadata_from_json_repr(data["metadata"])
            return RestfulFileEntry(metadata)
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )

    def query_file_metadata(self, data):
        request = Request(
            "POST", "/file/metadata", data.read(),
            headers={
                "Content-Type": "application/x-hdf5",
            }
        )

        response = self.execute_request(request)
        
        if response.status == httplibclient.OK:
            data = json.loads(response.read())
            return metadata_from_json_repr(data["metadata"])
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )

   
    def get_file_entry(self, uuid):
        request = Request(
            "GET", "/file/%s/metadata" % uuid
        )

        response = self.execute_request(request)

        if response.status == httplibclient.OK:
            data = json.loads(response.read())
            metadata = metadata_from_json_repr(data["metadata"])
            return RestfulFileEntry(metadata)
        elif response.status == httplibclient.NOT_FOUND:
            return None
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def get_file_content(self, uuid):
        request = Request(
            "GET", "/file/%s" % uuid
        )

        response = self.execute_request(request)
    
        if response.status == httplibclient.OK:
            return response
        elif response.status == httplibclient.NOT_FOUND:
            return None
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def remove_file_entry(self, uuid):
        request = Request(
            "DELETE", "/file/%s" % uuid
        )

        response = self.execute_request(request)
    
        if response.status == httplibclient.NO_CONTENT:
            return True
        elif response.status == httplibclient.NOT_FOUND:
            return False
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def get_sources(self):
        request = Request(
            "GET", "/source/"
        )

        response = self.execute_request(request)

        if response.status == httplibclient.OK:
            data = json.loads(response.read().decode('utf-8'))
            result = []
            for src in data["sources"]:
                parent = None
                if "parent" in src:
                    parent = src["parent"]
                result.append(oh5.Source(src["name"], values=src["values"], parent=parent))
            return result
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )
            
    def get_source(self, name):
        request = Request(
            "GET", "/source/by_name/%s"%name
        )

        response = self.execute_request(request)

        if response.status == httplibclient.OK:
            data = json.loads(response.read().decode('utf-8'))
            src = data["source"]
            if "parent" in src:
                parent = src["parent"]
            result = oh5.Source(src["name"], values=src["values"], parent=parent)
            return result
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def add_source(self, source):
        request = Request(
            "POST", "/source/", json.dumps({
                "source" : {
                    "name": source.name,
                    "values": dict(source),
                    "parent": source.parent
                }
            })
        )

        response = self.execute_request(request)

        if response.status == httplibclient.CREATED:
            pass
        elif response.status == httplibclient.CONFLICT:
            raise db.DatabaseError("duplicate entry")
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def update_source(self, source):
        request = Request(
            "PUT", "/source/", json.dumps({
                "source": {
                    "name": source.name,
                    "values": dict(source),
                    "parent": source.parent
                }
            })
        )

        response = self.execute_request(request)

        if response.status == httplibclient.NO_CONTENT:
            return
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )

    def remove_source(self, name):
        request = Request(
            "DELETE", "/source/%s" % name
        )

        response = self.execute_request(request)

        if response.status == httplibclient.NO_CONTENT:
            return True
        elif response.status == httplibclient.NOT_FOUND:
            return False
        elif response.status == httplibclient.CONFLICT:
            raise db.DatabaseError(
                "couldn't remove source, (it might be associated with files)"
            )
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )

    def get_parent_sources(self):
        request = Request(
            "GET", "/source/parents"
        )

        response = self.execute_request(request)

        if response.status == httplibclient.OK:
            data = json.loads(response.read().decode('utf-8'))
            result = []
            for src in data["sources"]:
                parent = None
                if "parent" in src:
                    parent = src["parent"]
                result.append(oh5.Source(src["name"], values=src["values"], parent=parent))
            return result
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )

    def get_sources_with_parent(self, parent):
        request = Request(
            "GET", "/source/with_parent/%s"%parent
        )

        response = self.execute_request(request)

        if response.status == httplibclient.OK:
            data = json.loads(response.read().decode('utf-8'))
            result = []
            for src in data["sources"]:
                parent = None
                if "parent" in src:
                    parent = src["parent"]
                result.append(oh5.Source(src["name"], values=src["values"], parent=parent))
            return result
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def execute_file_query(self, query):
        request = Request(
            "POST", "/query/file",
            data=query.to_json(),
            headers={
                "content-type": "application/json"
            }
        )

        response = self.execute_request(request)

        if response.status == httplibclient.OK:
            return RestfulFileResult(
                self,
                rows=json.loads(response.read())["rows"]
            )
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )
    
    def execute_attribute_query(self, query):
        request = Request(
            "POST", "/query/attribute",
            data=query.to_json(),
            headers={
                "content-type": "application/json"
            }
        )
        
        response = self.execute_request(request)

        if response.status == httplibclient.OK:
            return RestfulAttributeResult(
                rows=json.loads(response.read())["rows"]
            )
        else:
            raise db.DatabaseError(
                "Unhandled response code: %s" % response.status
            )

    
    def execute_request(self, req):
        conn = httplibclient.HTTPConnection(
            self._server_url.hostname,
            self._server_url.port
        )
        self._auth.add_credentials(req)
        try:
            conn.request(req.method, req.path, req.data, req.headers)
        except socket.error:
            raise db.DatabaseError(
                "Could not send request to %s" % self._server_url_str
            )
        return conn.getresponse()

class RestfulFileResult(db.FileResult):
    def __init__(self, db, rows=[]):
        self._db = db
        self._rows = rows
        self._iter = iter(rows)
        self._row = None
    
    def next(self):
        self._row = next(self._iter, None)
        return self._row is not None

    def size(self):
        return len(self._rows)
    
    def get_file_entry(self):
        if not self._row:
            raise LookupError("RestfulFileResult exhausted")
        return self._db.get_file_entry(self._row["uuid"])
    
    def get_uuid(self):
        if not self._row:
            raise LookupError("RestfulFileResult exhausted")
        return self._row["uuid"]

class RestfulAttributeResult(db.AttributeResult):
    def __init__(self, rows=[]):
        self._rows = rows
        self._iter = iter(rows)
        self._row = None

    def next(self):
        self._row = next(self._iter, None)
        return self._row is not None

    def size(self):
        return len(self._rows)
    
    def get(self, key):
        if not self._row:
            raise LookupError("RestfulAttributeResult exhausted")
        return self._row[key]
    
    def close(self):
        pass
    
class Auth(object):
    __meta__ = abc.ABCMeta

    @abc.abstractmethod
    def add_credentials(self, req):
        """add authorization credentials to the request

        :param req: a :class:`Request` to update
        """
        raise NotImplementedError()

class NoAuth(Auth):
    """no authentication
    """
    def add_credentials(self, req):
        pass

class KeyczarAuth(Auth):
    """authenicate by signing messages with Keyczar
    """
    def __init__(self, key_path, key_name=None):
        self._signer = keyczarcrypto.keyczar_signer.read(key_path)
        if key_name:
            self._key_name = key_name
        else:
            self._key_name = os.path.basename(key_path)

    def add_credentials(self, req):
        signable = create_signable_string(req)
        signature = self._signer.sign(signable)
        auth = "bdb-keyczar %s:%s" % (self._key_name, signature)
        req.headers["authorization"] = auth

def create_signable_string(req):
    """construct a signable string from a :class:`.Request`

    See :ref:`doc-rest-authentication` for details.
    """
    fragments = [req.method, req.path]
    for key in ("content-md5", "content-type", "date"):
        if key in req.headers:
            value = req.headers[key].strip()
            if value:
                fragments.append(value)

    return "\n".join(fragments)
