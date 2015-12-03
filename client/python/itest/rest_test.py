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

from keyczar import errors as kzerrors

from nose.tools import eq_, ok_, raises

from baltrad.bdbclient import rest
from baltrad.bdbcommon import oh5

from . import get_database

class TestKeyczarAuth(object):
    @raises(kzerrors.KeyczarError)
    def test_ctor_nx_key(self):
        rest.KeyczarAuth("/path/to/nxkey")
    
    @raises(kzerrors.KeyczarError)
    def test_ctor_invalid_key(self):
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
        ok_(credentials.startswith("bdb-keyczar testkey:AH"))

class TestRestfulDatabase(object):
    def setup(self):
        self.database = get_database()
    
    def teardown(self):
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
        eq_('sevil', source['NOD'])
        eq_('Vilebo', source['PLC'])
        eq_('SE48', source['RAD'])
        eq_('02570', source['WMO'])

    def test_add_source(self):
        values = {'NOD':'nisse', 'PLC':'Nisse town'}
        source = oh5.Source('nisse', values=values)
        self.database.add_source(source)
        
        result = self.get_source('nisse')
        eq_('nisse', source['NOD'])
        eq_('Nisse town', source['PLC'])
        
    def test_update_source(self):
        values = {'NOD':'nisse', 'PLC':'Nisse town'}
        source = oh5.Source('nisse', values=values)
        self.database.add_source(source)

        source['PLC'] = 'Nisses super town'
        self.database.update_source(source)

        result = self.get_source('nisse')
        eq_('nisse', source['NOD'])
        eq_('Nisses super town', source['PLC'])

    def test_remove_source(self):
        values = {'NOD':'nisse', 'PLC':'Nisse town'}
        source = oh5.Source('nisse', values=values)
        self.database.add_source(source)

        self.database.remove_source('nisse')
        result = self.get_source('nisse')
        eq_(None, result)


    def test_get_parent_sources(self):
        parent_sources = self.database.get_parent_sources()
        eq_(3, len(parent_sources))
        source = self.get_source_from_sources(parent_sources, 'by')
        eq_("by", source.name)
        eq_(None, source.parent)
        eq_('226', source['ORG'])
        eq_('UMMS', source['CCCC'])
        
        source = self.get_source_from_sources(parent_sources, 'se')
        eq_("se", source.name)
        eq_(None, source.parent)
        eq_('82', source['ORG'])
        eq_('ESWI', source['CCCC'])

        source = self.get_source_from_sources(parent_sources, 'pl')
        eq_("pl", source.name)
        eq_(None, source.parent)
        eq_('220', source['ORG'])
        eq_('SOWR', source['CCCC'])
        
    def test_get_sources_with_parent(self):
        sources = self.database.get_sources_with_parent("se")
        eq_(12, len(sources))
        source = self.get_source_from_sources(sources, 'sekir')
        eq_("sekir", source.name)
        eq_("se", source.parent)
        eq_("Kiruna", source["PLC"])
        eq_("SE40", source["RAD"])
        eq_("02032", source["WMO"])
        source = self.get_source_from_sources(sources, 'searl')
        eq_("searl", source.name)
        eq_("se", source.parent)
        eq_("Arlanda", source["PLC"])
        eq_("SE46", source["RAD"])
        eq_("02451", source["WMO"])
        
 
