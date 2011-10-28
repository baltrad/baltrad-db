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

import base64
from tempfile import NamedTemporaryFile

from .util import (
    HttpConflict,
    HttpNotFound,
    JsonResponse,
)

from ..backend import DuplicateEntry

def add_file(ctx):
    """add a file to the database

    the request body is expected to contain: ::

      { "data": `base64-encoded file content` }

    on success, the body contains: ::

      { "metadata": `file metadata` }

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.JsonResponse` with status
             *201 Created* and metadata in body
    :raise: :class:`baltrad.bdb.web.util.HttpConflict` when file already
            stored
    """
    data = ctx.request.get_json_data()
    
    with NamedTemporaryFile() as tmp:
        tmp.write(base64.standard_b64decode(data["data"]))
        tmp.flush()
        try:
            metadata = ctx.backend.store_file(tmp.name)
        except DuplicateEntry:
            raise HttpConflict("duplicate file entry")

    response = JsonResponse({"metadata": metadata.json_repr()}, status=201)
    response.headers["Location"] = ctx.make_url("file/" + metadata.bdb_uuid)

    return response

def get_file(ctx, uuid):
    """get file content from the database

    on success, the body contains: ::
    
      { "data": `base64-encoded file content` }

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.JsonResponse` with status
             *200 OK* and file content in body
    :raise: :class:`baltrad.bdb.web.util.HttpConflict` when file not found
    """
    data = ctx.backend.get_file(uuid)
    if data is None:
        raise HttpNotFound()

    return JsonResponse({"data" : base64.standard_b64encode(data)})

def get_file_metadata(ctx, uuid):
    """get file metadata from the database

    on success, the body contains: ::

      { "metadata": `file metadata` }

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.JsonResponse` with status
             *200 OK* and metadata in body
    :raise: :class:`baltrad.bdb.web.util.HttpNotFound` when file not found
    """
    metadata = ctx.backend.get_file_metadata(uuid)
    if metadata is None:
        raise HttpNotFound()

    return JsonResponse({"metadata": metadata.json_repr()})

def remove_file(ctx, uuid):
    """remove the file from the database

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.JsonResponse` with status *200 OK*
             with no body
    :raise: :class:`baltrad.bdb.web.util.HttpNotFound` when file not found
    """
    found = ctx.backend.remove_file(uuid)
    if not found:
        raise HttpNotFound()
    
    return JsonResponse("")

def get_sources(ctx):
    """get a list of sources

    on success, the body contains: ::

      { "sources": [ {"_name": "name", "key": "value", ...}, ...] }

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.JsonResponse` with status *200 OK*
    """
    sources = ctx.backend.get_sources()
    return JsonResponse({"sources": [dict(src) for src in sources]})
