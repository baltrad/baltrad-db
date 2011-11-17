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
import shutil
from tempfile import NamedTemporaryFile

from .util import (
    HttpConflict,
    HttpNotFound,
    JsonResponse,
    Response,
)

from ..backend import AttributeQuery, DuplicateEntry, FileQuery

def add_file(ctx):
    """add a file to the database

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.JsonResponse` with status
             *201 Created* and metadata in body
    :raise: :class:`baltrad.bdb.web.util.HttpConflict` when file already
            stored

    See :ref:`doc-rest-op-store-file` for details
    """
    with NamedTemporaryFile() as tmp:
        shutil.copyfileobj(ctx.request.stream, tmp)
        tmp.flush()
        try:
            metadata = ctx.backend.store_file(tmp.name)
        except DuplicateEntry:
            raise HttpConflict("duplicate file entry")

    response = JsonResponse(
        {"metadata": metadata.json_repr()},
        status=httplib.CREATED
    )
    response.headers["Location"] = ctx.make_url("file/" + metadata.bdb_uuid)

    return response

def get_file(ctx, uuid):
    """get file content from the database

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.Response` with status
             *200 OK* and file content in body
    :raise: :class:`baltrad.bdb.web.util.HttpConflict` when file not found

    See :ref:`doc-rest-op-get-file` for details
    """
    data = ctx.backend.get_file(uuid)
    if data is None:
        raise HttpNotFound()

    return Response(data, content_type="application/x-hdf5")

def get_file_metadata(ctx, uuid):
    """get file metadata from the database

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.JsonResponse` with status
             *200 OK* and metadata in body
    :raise: :class:`baltrad.bdb.web.util.HttpNotFound` when file not found

    See :ref:`doc-rest-op-get-file-metadata` for details
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

    See :ref:`doc-rest-op-remove-file` for details
    """
    found = ctx.backend.remove_file(uuid)
    if not found:
        raise HttpNotFound()
    
    return JsonResponse("")

def get_sources(ctx):
    """get a list of sources

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.JsonResponse` with status *200 OK*

    See :ref:`doc-rest-op-get-sources` for details
    """
    sources = ctx.backend.get_sources()
    return JsonResponse({"sources": [dict(src) for src in sources]})

def query_file(ctx):
    """execute a file query

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.JsonResponse` with status *200 OK*

    See :ref:`doc-rest-op-query-file` for details
    """

    data = ctx.request.get_json_data()

    query = FileQuery()
    query.filter = data.get("filter", query.filter)
    query.order = data.get("order", query.order)
    query.limit = data.get("limit", query.limit)
    query.skip = data.get("skip", query.skip)

    result = query.execute(ctx.backend)

    return JsonResponse({"data": result})

def query_attribute(ctx):
    """execute an attribute query

    :param ctx: the request context
    :type ctx: :class:`baltrad.bdb.web.util.RequestContext`
    :return: :class:`baltrad.bdb.web.util.JsonResponse` with status *200 OK*

    See :ref:`doc-rest-op-query-attribute` for details
    """

    data = ctx.request.get_json_data()

    query = AttributeQuery()
    query.fetch = data.get("fetch", query.fetch)
    query.filter = data.get("filter", query.filter)
    query.distinct = data.get("distinct", query.distinct)
    query.order = data.get("order", query.order)
    query.group = data.get("group", query.group)
    query.limit = data.get("limit", query.limit)
    query.skip = data.get("skip", query.skip)

    result = query.execute(ctx.backend)

    return JsonResponse({"data": result})
