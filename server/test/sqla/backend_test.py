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

import datetime

import mock
from nose.tools import eq_, raises
from sqlalchemy import engine, sql

from baltrad.bdbcommon import oh5
from baltrad.bdbserver import config
from baltrad.bdbserver.sqla import backend, schema, storage

from ..test_util import check_instance, pop_last_call

class TestSqlaAlchemyBackend(object):
    def setup(self):
        self.conn = mock.MagicMock(spec=[
            "begin", "close", "execute", "__exit__", "__enter__"
        ])
        self.conn.__enter__.return_value = self.conn
        self.tx = mock.MagicMock(spec=[
            "commit", "rollback", "__enter__", "__exit__"
        ])
        self.conn.begin.return_value = self.tx
        self.engine = mock.Mock(spec=engine.base.Engine)
        self.engine.driver = "drivername"
        self.storage = mock.Mock(spec=storage.FileStorage)
        self.backend = backend.SqlAlchemyBackend(
            self.engine,
            storage=self.storage
        )

    def test_driver(self):
        eq_("drivername", self.backend.driver)
    
    @mock.patch("baltrad.bdbserver.sqla.storage.FileStorage.impl_from_conf")
    def test_from_conf(self, storage_from_conf):
        # store original method, we are going to patch the class
        storage_from_conf.return_value = mock.sentinel.storage
        from_conf = self.backend.from_conf
        conf = config.Properties({
            "baltrad.bdb.server.backend.sqla.uri": mock.sentinel.uri,
            "baltrad.bdb.server.backend.sqla.storage.type": "mock",
        })

        with mock.patch(
            "baltrad.bdbserver.sqla.backend.SqlAlchemyBackend"
        ) as ctor:
            ctor.return_value = mock.sentinel.backend
            result = from_conf(conf)
            ctor.assert_called_once_with(
                mock.sentinel.uri,
                storage=mock.sentinel.storage
            )
            storage_from_conf.assert_called_with("mock", conf)
            eq_(mock.sentinel.backend, result)
    
    @raises(config.Error)
    def test_from_conf_missing_uri(self):
        conf = config.Properties({
        })
        backend.SqlAlchemyBackend.from_conf(conf)
    
    @mock.patch("baltrad.bdbserver.sqla.storage.FileStorage.impl_from_conf")
    def test_from_conf_default_storage(self, storage_from_conf):
        # store original method, we are going to patch the class
        storage_from_conf.return_value = mock.sentinel.storage
        from_conf = self.backend.from_conf
        conf = config.Properties({
            "baltrad.bdb.server.backend.sqla.uri": mock.sentinel.uri,
        })

        with mock.patch(
            "baltrad.bdbserver.sqla.backend.SqlAlchemyBackend"
        ) as ctor:
            ctor.return_value = mock.sentinel.backend
            result = from_conf(conf)
            ctor.assert_called_once_with(mock.sentinel.uri, storage=mock.sentinel.storage)
            storage_from_conf.assert_called_with("db", conf)
            eq_(mock.sentinel.backend, result)        
    
    def test_store_file(self):
        metadata = mock.sentinel.metadata
        path = "/path/to/file"
        self.backend.metadata_from_file = mock.Mock(return_value=metadata)

        result = self.backend.store_file(path)
        self.backend.metadata_from_file.assert_called_once_with(path)
        self.storage.store.called_once_with(self.backend, metadata, path)
        eq_(metadata, result)
    
    def test_get_file(self):
        uuid = mock.sentinel.uuid
        self.storage.read.return_value = mock.sentinel.content

        result = self.backend.get_file(uuid)
        self.storage.read.assert_called_once_with(self.backend, uuid)
        eq_(mock.sentinel.content, result)
    
    def test_get_file_not_found(self):
        uuid = mock.sentinel.uuid
        self.storage.read.side_effect = storage.FileNotFound()

        eq_(None, self.backend.get_file(uuid))
    
    def _test_remove_file(self):
        uuid = mock.sentinel.uuid
        self._storage.remove(uuid)

        sqlresult = self.conn.execute.return_value
        sqlresult.rowcount = 1
        self.backend.get_connection = mock.Mock(return_value=self.conn)
        
        result = self.backend.remove_file(uuid)
        self.backend.get_connection.assert_called_once_with()
        self.conn.begin.assert_called_once_with()
        self.storage.remove.assert_called_once_with(self.backend, uuid)
        self.conn.execute.assert_called_once_with(
            check_instance(sql.expression.Delete)
        )
        self.tx.__exit__.assert_called_once()
        self.conn.__exit__.assert_called_once()
        eq_(True, result)
    
    def test_remove_file(self):
        uuid = mock.sentinel.uuid

        result = self.backend.remove_file(uuid)
        self.storage.remove.assert_called_once_with(self.backend, uuid)
        eq_(True, result)
        
    def test_remove_file_not_found(self):
        uuid = mock.sentinel.uuid
        self.storage.remove.side_effect = storage.FileNotFound()

        result = self.backend.remove_file(uuid)
        self.storage.remove.assert_called_once_with(self.backend, uuid)
        eq_(False, result)
    
    def test_remove_all_files(self):
        self.backend.get_connection = mock.Mock(return_value=self.conn)
        self.backend.remove_file = mock.Mock()
        self.conn.execute.return_value = [
            {schema.files.c.uuid: mock.sentinel.uuid1},
            {schema.files.c.uuid: mock.sentinel.uuid2},
        ]

        self.backend.remove_all_files()
        self.conn.execute.assert_called_once_with(
            check_instance(sql.expression.Select)
        )
        self.backend.remove_file.assert_called_with(mock.sentinel.uuid2)
        pop_last_call(self.backend.remove_file)
        self.backend.remove_file.assert_called_with(mock.sentinel.uuid1)
        self.conn.__exit__.assert_called_once()
    
    def test_delete_metadata(self):
        sqlresult = self.conn.execute.return_value
        sqlresult.rowcount = 1
        
        result = self.backend.delete_metadata(self.conn, mock.sentinel.uuid)
        self.conn.execute.assert_called_once_with(
            check_instance(sql.expression.Delete)
        )
        eq_(True, result)
        
    def test_delete_metadata_not_found(self):
        sqlresult = self.conn.execute.return_value
        sqlresult.rowcount = 0
        
        result = self.backend.delete_metadata(self.conn, mock.sentinel.uuid)
        self.conn.execute.assert_called_once_with(
            check_instance(sql.expression.Delete)
        )
        eq_(False, result)

def test_attribute_sql_values_str():
    attr = oh5.Attribute("name", "strval")
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_str": "strval"
    }
    eq_(expected, result)

def test_attribute_sql_values_int():
    attr = oh5.Attribute("name", 123)
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_int": 123
    }
    eq_(expected, result)

def test_attribute_sql_values_long():
    attr = oh5.Attribute("name", 123L)
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_int": 123L
    }
    eq_(expected, result)

def test_attribute_sql_values_float():
    attr = oh5.Attribute("name", 1.23)
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_double": 1.23
    }
    eq_(expected, result)

def test_attribute_sql_values_date():
    attr = oh5.Attribute("name", "20111213")
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_str": "20111213",
        "value_date": datetime.date(2011, 12, 13)
    }
    eq_(expected, result)

def test_attribute_sql_values_invalid_date():
    attr = oh5.Attribute("name", "20111313")
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_str": "20111313",
    }
    eq_(expected, result)

def test_attribute_sql_values_time():
    attr = oh5.Attribute("name", "131415")
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_str": "131415",
        "value_time": datetime.time(13, 14, 15)
    }
    eq_(expected, result)

def test_attribute_sql_values_invalid_time():
    attr = oh5.Attribute("name", "241415")
    result = backend._get_attribute_sql_values(attr)
    expected = {
        "value_str": "241415",
    }
    eq_(expected, result)
