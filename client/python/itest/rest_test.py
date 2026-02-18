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
import os

import pytest

from baltrad.bdbclient import rest
from baltrad.bdbcommon import oh5

from .conftest import get_database

class TestKeyczarAuth(object):
    def test_ctor_nx_key(self):
        with pytest.raises(Exception):
            rest.KeyczarAuth("/path/to/nxkey")
    
    def test_ctor_invalid_key(self):
        with pytest.raises(Exception):
            rest.KeyczarAuth(
                os.path.join(
                    os.path.dirname(__file__),
                    "fixture",
                    "invalidkey"
                )
            )
    
    def test_sign(self):
        auth = rest.KeyczarAuth(
            os.path.join(
                os.path.dirname(__file__),
                "fixture",
                "testkey"
            )
        )

        req = rest.Request(
            "GET",
            "/path/to/resource",
            headers={
                "content-type": "application/json",
                "date": "Tue, 10 Jan 2012 19:03:34 GMT",
                "content-md5": "a23ae94a35970e9689d30a56d954ef36",
            }
        )
        auth.add_credentials(req)
        credentials = req.headers["authorization"]
        assert(credentials.startswith("bdb-keyczar testkey:AH"))

class TestRestfulDatabase(object):
    def setup_method(self):
        self.database = get_database()
    
    def teardown_method(self):
        self.database.remove_source('nisse')
        self.database = None
    
    def get_source_from_sources(self, sources, name):
        for s in sources:
            if s.name == name:
                return s
        return None
    
    def get_source(self, name):
        sources = self.database.get_sources()
        return self.get_source_from_sources(sources, name)
    
    def test_get_sources(self):
        sources = self.database.get_sources()
        source = self.get_source_from_sources(sources, 'sevil')
        assert(source['NOD'] == 'sevil')
        assert(source['PLC'] == 'Vilebo')
        assert(source['RAD'] == 'SE48')
        assert(source['WMO'] == '02570')

    def test_get_source(self):
        #<se CCCC="ESWI" org="82" cty="643">
        source = self.database.get_source("se")
        assert(source.name == 'se')
        assert(source['CCCC'] == 'ESWI')
        assert(source['ORG'] == '82')
        assert(source['CTY'] == '643')

        source = self.database.get_source("sevil")
        assert(source['NOD'] == 'sevil')
        assert(source['PLC'] == 'Vilebo')
        assert(source['RAD'] == 'SE48')
        assert(source['WMO'] == '02570')

    def test_add_source(self):
        values = {'NOD':'nisse', 'PLC':'Nisse town'}
        source = oh5.Source('nisse', values=values)
        self.database.add_source(source)
        
        result = self.get_source('nisse')
        assert(source['NOD'] == 'nisse')
        assert(source['PLC'] == 'Nisse town')
        
    def test_update_source(self):
        values = {'NOD':'nisse', 'PLC':'Nisse town'}
        source = oh5.Source('nisse', values=values)
        self.database.add_source(source)

        source['PLC'] = 'Nisses super town'
        self.database.update_source(source)

        result = self.get_source('nisse')
        assert(source['NOD'] == 'nisse')
        assert(source['PLC'] == 'Nisses super town')

    def test_remove_source(self):
        values = {'NOD':'nisse', 'PLC':'Nisse town'}
        source = oh5.Source('nisse', values=values)
        self.database.add_source(source)

        self.database.remove_source('nisse')
        result = self.get_source('nisse')
        assert(result is None)


    def test_get_parent_sources(self):
        parent_sources = self.database.get_parent_sources()
        assert(len(parent_sources) == 3)
        source = self.get_source_from_sources(parent_sources, 'by')
        assert(source.name == "by")
        assert(source.parent is None)
        assert(source['ORG'] == '226')
        assert(source['CCCC'] == 'UMMS')
        
        source = self.get_source_from_sources(parent_sources, 'se')
        assert(source.name == "se")
        assert(source.parent is None)
        assert(source['ORG'] == '82')
        assert(source['CCCC'] == 'ESWI')

        source = self.get_source_from_sources(parent_sources, 'pl')
        assert(source.name == "pl")
        assert(source.parent is None)
        assert(source['ORG'] == '220')
        assert(source['CCCC'] == 'SOWR')
        
    def test_get_sources_with_parent(self):
        sources = self.database.get_sources_with_parent("se")
        assert(len(sources) == 12)
        source = self.get_source_from_sources(sources, 'sekir')
        assert(source.name == "sekir")
        assert(source.parent == "se")
        assert(source["PLC"] == "Kiruna")
        assert(source["RAD"] == "SE40")
        assert(source["WMO"] == "02032")
        source = self.get_source_from_sources(sources, 'searl')
        assert(source.name == "searl")
        assert(source.parent == "se")
        assert(source["PLC"] == "Arlanda")
        assert(source["RAD"] == "SE46")
        assert(source["WMO"] == "02451")

 
