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
import pytest

class TestMain:
    def setup_method(self):
        self.fixtures = os.path.join(os.path.dirname(__file__), "fixtures")

    def teardown_method(self):
        pass

    def test_read_config(self):
        configfile = "%s/baseconfig.conf"%self.fixtures
        conf = main.read_config(configfile)
        print("TYPE: %s"%str(type(conf)))
        assert(conf["baltrad.bdb.server.type"] == "werkzeug")
        assert(conf["baltrad.bdb.server.uri"] == "http://localhost:8090")
        assert(conf["baltrad.bdb.server.backend.type"] == "sqla")
        assert(conf["baltrad.bdb.server.backend.sqla.uri"] == "postgresql://baltrad:baltrad@127.0.0.1/baltrad")
        assert(conf["baltrad.bdb.server.backend.sqla.pool_size"] == "10")
        assert(conf["baltrad.bdb.server.backend.sqla.storage.type"] == "db")
        assert(conf["baltrad.bdb.server.auth.providers"] == "noauth, keyczar")
        assert(conf["baltrad.bdb.server.auth.keyczar.keystore_root"] == "/tmp")
        assert(conf["baltrad.bdb.server.auth.keyczar.keys.smurf"] == "smurf.pub")
        assert(conf["baltrad.bdb.server.enable_remove_all_files"])
        
    def test_get_logging_level_no_property(self):
        conf = config.Properties({})
        result = main.get_logging_level(conf)
        assert(result == logging.INFO)

    def test_get_logging_level_unknown_value(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"SIG"})
        result = main.get_logging_level(conf)
        assert(result == logging.INFO)
        
    def test_get_logging_level_DEBUG(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"DEBUG"})
        result = main.get_logging_level(conf)
        assert(result == logging.DEBUG)

    def test_get_logging_level_INFO(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"INFO"})
        result = main.get_logging_level(conf)
        assert(result == logging.INFO)

    def test_get_logging_level_WARN(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"WARN"})
        result = main.get_logging_level(conf)
        assert(result == logging.WARN)

    def test_get_logging_level_WARNING(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"WARNING"})
        result = main.get_logging_level(conf)
        assert(result == logging.WARNING)

    def test_get_logging_level_ERROR(self):
        conf = config.Properties({"baltrad.bdb.server.log.level":"ERROR"})
        result = main.get_logging_level(conf)
        assert(result == logging.ERROR)
