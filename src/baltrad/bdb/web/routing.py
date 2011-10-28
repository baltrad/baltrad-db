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

class UuidConverter(BaseConverter):
    def __init__(self, url_map):
        self.url_map = url_map
        self.regex = "[0-9a-fA-F]{8,8}-([0-9a-fA-F]{4,4}-){3,3}[0-9a-fA-F]{12,12}"
    
    def to_python(self, value):
        return uuid.UUID(value)
    
    def from_python(self, value):
        return value.get_hex()

URL_MAP = Map(
    rules=[
        Submount("/file", [
            Rule("/", methods=["POST"],
                endpoint="handler.add_file"
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

        ]),
        Submount("/source", [
            Rule("/", methods=["GET"],
                endpoint="handler.get_sources"
            ),
        ])
    ],
    converters={
        "uuid": UuidConverter
    }
)

def get_handler(handler_str):
    return import_string("baltrad.bdb.web." + handler_str)
