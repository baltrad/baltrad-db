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

import json
import httplib

from werkzeug.wrappers import (
    BaseRequest,
    BaseResponse,
    CommonRequestDescriptorsMixin,
    CommonResponseDescriptorsMixin,
)

from werkzeug.exceptions import HTTPException

class JsonRequestMixin(object):
    """Request mixin providing JSON-decoding of request body
    """
    
    def get_json_data(self):
        """try JSON decoding :attr:`self.data`

        :return: the decoded data
        :raise: :class:`HttpBadRequest` if decoding fails
        """
        try:
            return json.loads(self.data)
        except ValueError:
            raise HttpBadRequest("invalid json: " + self.data)

class Request(BaseRequest,
              CommonRequestDescriptorsMixin,
              JsonRequestMixin):
    def __init__(self, environ):
        BaseRequest.__init__(self, environ)

class RequestContext(object):
    def __init__(self, request, backend):
        self.request = request
        self.backend = backend
    
    def make_url(self, path):
        return "/" + path

class Response(BaseResponse,
               CommonResponseDescriptorsMixin):
    def __init__(self, response, content_type="text/plain", status=httplib.OK):
        BaseResponse.__init__(
            self, response,
            content_type=content_type,
            status=status
        )

class NoContentResponse(BaseResponse,
                        CommonResponseDescriptorsMixin):
    
    def __init__(self):
        BaseResponse.__init__(self, None, status=httplib.NO_CONTENT)

class JsonResponse(BaseResponse,
                   CommonResponseDescriptorsMixin):
    def __init__(self, response, status=httplib.OK):
        if not isinstance(response, basestring):
            response = json.dumps(response, allow_nan=False)
        BaseResponse.__init__(
            self, response,
            content_type="application/json",
            status=status,
        )

class HttpBadRequest(HTTPException):
    code = httplib.BAD_REQUEST

class HttpNotFound(HTTPException):
    code = httplib.NOT_FOUND
    
class HttpConflict(HTTPException):
    code = httplib.CONFLICT

class HttpUnauthorized(HTTPException):
    code = httplib.UNAUTHORIZED
