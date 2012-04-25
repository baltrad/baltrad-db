'''
Copyright (C) 2010- Swedish Meteorological and Hydrological Institute (SMHI)
 
 This file is part of baltrad-db.
 
 baltrad-db is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as
 published by the Free Software Foundation, either version 3 of the
 License, or (at your option) any later version.
 
 baltrad-db is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License
 along with baltrad-db. If not, see <http://www.gnu.org/licenses/>.
'''

## Tests the main functions
## @file
## @author Anders Henja, SMHI
## @date 2012-04-25

from baltrad.bdbserver import main
from baltrad.bdbserver import config
import os, logging
from nose.tools import eq_, ok_, raises

class main_test(object):
    def setup(self):
        self.fixtures = os.path.join(os.path.dirname(__file__), "fixtures")

    def tearDown(self):
        pass

    def test_read_config(self):
        configfile = "%s/baseconfig.conf"%self.fixtures
        conf = main.read_config(configfile)
        print "TYPE: %s"%`type(conf)`
        eq_("werkzeug", conf["baltrad.bdb.server.type"])
        eq_("http://localhost:8090", conf["baltrad.bdb.server.uri"])
        eq_("sqla", conf["baltrad.bdb.server.backend.type"])
        eq_("postgresql://baltrad:baltrad@127.0.0.1/baltrad", conf["baltrad.bdb.server.backend.sqla.uri"])
        eq_("10", conf["baltrad.bdb.server.backend.sqla.pool_size"])
        eq_("db", conf["baltrad.bdb.server.backend.sqla.storage.type"])
        eq_("noauth, keyczar", conf["baltrad.bdb.server.auth.providers"])
        eq_("/tmp", conf["baltrad.bdb.server.auth.keyczar.keystore_root"])
        eq_("smurf.pub", conf["baltrad.bdb.server.auth.keyczar.keys.smurf"])
        ok_(conf["baltrad.bdb.server.enable_remove_all_files"])
        
    def test_get_logging_level_no_property(self):
        conf = config.Properties({})
        result = main.get_logging_level(conf)
        eq_(logging.INFO, result)

    def test_get_logging_level_unknown_value(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"SIG"})
        result = main.get_logging_level(conf)
        eq_(logging.INFO, result)
        
    def test_get_logging_level_DEBUG(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"DEBUG"})
        result = main.get_logging_level(conf)
        eq_(logging.DEBUG, result)

    def test_get_logging_level_INFO(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"INFO"})
        result = main.get_logging_level(conf)
        eq_(logging.INFO, result)

    def test_get_logging_level_WARN(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"WARN"})
        result = main.get_logging_level(conf)
        eq_(logging.WARN, result)

    def test_get_logging_level_WARNING(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"WARNING"})
        result = main.get_logging_level(conf)
        eq_(logging.WARNING, result)

    def test_get_logging_level_ERROR(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"ERROR"})
        result = main.get_logging_level(conf)
        eq_(logging.ERROR, result)
        