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

from baltrad.bdbcommon import expr, filter
from baltrad.bdbcommon.oh5 import Source

from .util import (
    HttpConflict,
    HttpForbidden,
    HttpNotFound,
    JsonResponse,
    NoContentResponse,
    Response,
)

from ..backend import (
    AttributeQuery,
    DuplicateEntry,
    FileQuery,
    IntegrityError,
)

def add_file(ctx):
    """add a file to the database

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.JsonResponse` with status
             *201 Created* and metadata in body
    :raise: :class:`~.util.HttpConflict` when file already
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
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.Response` with status
             *200 OK* and file content in body
    :raise: :class:`~.util.HttpConflict` when file not found

    See :ref:`doc-rest-op-get-file` for details
    """
    data = ctx.backend.get_file(uuid)
    if data is None:
        raise HttpNotFound()

    return Response(data, content_type="application/x-hdf5")

def get_file_metadata(ctx, uuid):
    """get file metadata from the database

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.JsonResponse` with status
             *200 OK* and metadata in body
    :raise: :class:`~.util.HttpNotFound` when file not found

    See :ref:`doc-rest-op-get-file-metadata` for details
    """
    metadata = ctx.backend.get_file_metadata(uuid)
    if metadata is None:
        raise HttpNotFound()
    return JsonResponse({"metadata": metadata.json_repr()})

def remove_file(ctx, uuid):
    """remove the file from the database

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.NoContentResponse`
    :raise: :class:`~.util.HttpNotFound` when file not found

    See :ref:`doc-rest-op-remove-file` for details
    """
    found = ctx.backend.remove_file(uuid)
    if not found:
        raise HttpNotFound()
    
    return NoContentResponse()

def remove_all_files(ctx):
    """remove all files from the database

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.NoContentResponse`
    """
    if not ctx.enable_remove_all_files:
        raise HttpForbidden()
    ctx.backend.remove_all_files()
    return NoContentResponse()

def get_sources(ctx):
    """get a list of sources

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.JsonResponse` with status *200 OK*

    See :ref:`doc-rest-op-get-sources` for details
    """
    sources = ctx.backend.get_source_manager().get_sources()
    return JsonResponse({
        "sources": [{"name": src.name, "values": dict(src)} for src in sources]
    })

def add_source(ctx):
    """add a source to the database

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.JsonResponse` with status *201 CREATED*

    See :ref:`doc-rest-op-add-source` for details
    """
    data = ctx.request.get_json_data()["source"]

    source = Source(data["name"], values=data["values"])
    ctx.backend.get_source_manager().add_source(source)
    response = Response("", status=httplib.CREATED)
    response.headers["Location"] = ctx.make_url("source/" + source.name)
    return response

def update_source(ctx, name):
    """update a source in the database

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.JsonResponse` with status *204 NO CONTENT*

    See :ref:`doc-rest-op-update-source` for details
    """
    data = ctx.request.get_json_data()["source"]

    source = Source(data["name"], values=data["values"])

    try:
        ctx.backend.get_source_manager().update_source(name, source)
        response = NoContentResponse()
        response.headers["Location"] = ctx.make_url("source/" + name)
    except LookupError:
        response = Response("", status=httplib.NOT_FOUND)
    except DuplicateEntry:
        response = Response("", status=httplib.CONFLICT)

    return response

def remove_source(ctx, name):
    """remove a source from the database

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.JsonResponse` with status *204 NO CONTENT*

    See :ref:`doc-rest-op-remove-source` for details
    """
    try:
        if ctx.backend.get_source_manager().remove_source(name):
            response = NoContentResponse()
        else:
            response = Response("", status=httplib.NOT_FOUND)
    except IntegrityError:
        response = Response("", status=httplib.CONFLICT)
    
    return response

def get_filters(ctx):
    """get a list of filters

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.JsonResponse` with status *200 OK*

    See :ref:`doc-rest-op-get-filters` for details
    """
    filter_names = ctx.backend.get_filter_manager().get_filter_names()
    return JsonResponse({
        "filters": filter_names
    })

def get_filter(ctx, name):
    """get a filter identified by `name`

    See :ref:`doc-rest-op-get-filter` for details

    :param ctx: the :class:`~.util.RequestContext`
    :param name: the name of the filter to fetch
    :return: :class:`~.util.JsonResponse` with status *200 OK*
    """
    flt = ctx.backend.get_filter_manager().get_filter(name)
    return JsonResponse({
        "filter": {
            "name": flt.name,
            "expression": expr.wrap_json(flt.expression)
        }
    })

def add_filter(ctx):
    """add a filter

    See :ref:`doc-rest-op-add-filter` for details

    :param ctx: the :class:`~.util.RequestContext`
    :return: :class:`~.util.JsonResponse` with status *201 CREATED*
    """
    data = ctx.request.get_json_data().get("filter")
    flt = filter.Filter(
        data.get("name"),
        expr.unwrap_json(data.get("expression")),
    )
    ctx.backend.get_filter_manager().add_filter(flt)
    response = JsonResponse("", status=httplib.CREATED)
    response.headers["Location"] = ctx.make_url("filter/%s" % flt.name)
    return response

def update_filter(ctx, name):
    """update a filter

    See :ref:`doc-rest-op-update-filter` for details

    :param ctx: the :class:`~.util.RequestContext`
    :param name: name of the filter to update
    :return: :class:`~.util.NoContentResponse`
    """
    data = ctx.request.get_json_data().get("filter")
    flt = filter.Filter(
        name,
        expr.unwrap_json(data.get("expression")),
    )
    try:
        ctx.backend.get_filter_manager().update_filter(flt)
    except LookupError:
        raise HttpNotFound()
    return NoContentResponse()

def remove_filter(ctx, name):
    """remove a filter

    See :ref:`doc-rest-op-remove-filter` for details

    :param ctx: the :class:`~.util.RequestContext`
    :param name: name of the filter to update
    :return: :class:`~.util.NoContentResponse`
    :raise: :class:`~.util.HttpNotFound` if filter didn't exist
    """
    if not ctx.backend.get_filter_manager().remove_filter(name):
        raise HttpNotFound()
    return NoContentResponse()

def query_file(ctx):
    """execute a file query

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.JsonResponse` with status *200 OK*

    See :ref:`doc-rest-op-query-file` for details
    """

    data = ctx.request.get_json_data()

    query = FileQuery()
    if "filter" in data:
        query.filter = expr.unwrap_json(data.get("filter"))
    query.order = [expr.unwrap_json(xpr) for xpr in data.get("order", [])]
    query.limit = data.get("limit", query.limit)
    query.skip = data.get("skip", query.skip)

    result = query.execute(ctx.backend)

    return JsonResponse({"rows": result})

def query_attribute(ctx):
    """execute an attribute query

    :param ctx: the request context
    :type ctx: :class:`~.util.RequestContext`
    :return: :class:`~.util.JsonResponse` with status *200 OK*

    See :ref:`doc-rest-op-query-attribute` for details
    """

    data = ctx.request.get_json_data()

    query = AttributeQuery()
    for key, value in data.get("fetch", {}).iteritems():
        query.fetch[key] = expr.unwrap_json(value)
    if "filter" in data:
        query.filter = expr.unwrap_json(data.get("filter"))
    query.distinct = data.get("distinct", query.distinct)
    query.order = [expr.unwrap_json(xpr) for xpr in data.get("order", [])]
    query.group = [expr.unwrap_json(xpr) for xpr in data.get("group", [])]
    query.limit = data.get("limit", query.limit)
    query.skip = data.get("skip", query.skip)

    result = query.execute(ctx.backend)

    return JsonResponse({"rows": result})
