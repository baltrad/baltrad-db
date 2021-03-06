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

import uuid

from werkzeug.routing import BaseConverter, Map, Rule, Submount
from werkzeug.utils import import_string
import datetime

class UuidConverter(BaseConverter):
    def __init__(self, url_map):
        super(UuidConverter, self).__init__(url_map)
        self.url_map = url_map
        self.regex = "[0-9a-fA-F]{8,8}-([0-9a-fA-F]{4,4}-){3,3}[0-9a-fA-F]{12,12}"
    
    def to_python(self, value):
        return uuid.UUID(value)
    
    def from_python(self, value):
        return value.get_hex()

class DatetimeConverter(BaseConverter):
    def __init__(self, url_map):
        super(DatetimeConverter, self).__init__(url_map)
        self.url_map = url_map
        self.regex = "[0-9]{14,14}"
    
    def to_python(self, value):
        a = value
        return datetime.datetime(int(a[:4]),int(a[4:6]),int(a[6:8]),int(a[8:10]),int(a[10:12]),int(a[12:])) 
    
    def from_python(self, value):
        return value.strftime("%Y%m%d%H%M%S")
    

URL_MAP = Map(
    rules=[
        Submount("/file", [
            Rule("/", methods=["POST"],
                endpoint="handler.add_file"
            ),
            Rule("/", methods=["DELETE"],
                endpoint="handler.remove_all_files"
            ),
            Rule("/metadata", methods=["POST"],
                endpoint="handler.query_file_metadata"
            ),
            Rule("/count/<int:limit>/<int:nritems>", 
                 methods=["DELETE"], 
                 endpoint="handler.remove_files_by_count"
            ),
            Rule("/age/<datetime:dt>/<int:nritems>",
                 methods=["DELETE"],
                 endpoint="handler.remove_files_by_age"
            ),                           
            Rule("/count", methods=["GET"],
                 endpoint="handler.file_count"
            ),
            Rule("/<uuid:uuid>", methods=["GET"],
                endpoint="handler.get_file"
            ),
            Rule("/<uuid:uuid>", methods=["DELETE"],
                endpoint="handler.remove_file"
            ),
            Rule("/<uuid:uuid>/metadata", methods=["GET"],
                endpoint="handler.get_file_metadata"
            ),
        ]),
        Submount("/query", [
            Rule("/file", methods=["POST"],
                endpoint="handler.query_file"
            ),
            Rule("/attribute", methods=["POST"],
                endpoint="handler.query_attribute"
            ),
        ]),
        Submount("/source", [
            Rule("/", methods=["GET"],
                endpoint="handler.get_sources"
            ),
            Rule("/by_name/<string:name>", methods=["GET"],
                endpoint="handler.get_source"
            ),
            Rule("/", methods=["POST"],
                endpoint="handler.add_source"
            ),
            Rule("/", methods=["PUT"],
                endpoint="handler.update_source"
            ),
            Rule("/<string:name>", methods=["DELETE"],
                endpoint="handler.remove_source"
            ),
            Rule("/parents", methods=["GET"],
                endpoint="handler.get_parent_sources"
            ),
            Rule("/with_parent/<string:parent>", methods=["GET"],
                 endpoint="handler.get_sources_with_parent"
            )
        ]),
        Submount("/filter", [
            Rule("/", methods=["GET"],
                endpoint="handler.get_filters"
            ),
            Rule("/", methods=["POST"],
                endpoint="handler.add_filter"
            ),
            Rule("/<string:name>", methods=["GET"],
                endpoint="handler.get_filter"
            ),
            Rule("/<string:name>", methods=["PUT"],
                endpoint="handler.update_filter"
            ),
            Rule("/<string:name>", methods=["DELETE"],
                endpoint="handler.remove_filter"
            ),
        ])
    ],
    converters={
        "uuid": UuidConverter,
        "datetime":DatetimeConverter
    }
)

def get_handler(handler_str):
    return import_string("baltrad.bdbserver.web." + handler_str)
