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

import errno
import uuid

from nose.tools import eq_, ok_, raises
import mock
from sqlalchemy import sql

from baltrad.bdbcommon import oh5
from baltrad.bdbserver import config
from baltrad.bdbserver.sqla import backend, storage

from ..test_util import check_instance

class MockConnection(mock.Mock):
    def __init__(self, *args, **kw):
        kw["spec_set"] = ["begin", "close", "execute"]
        mock.Mock.__init__(self, *args, **kw)

    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

class MockTransaction(mock.Mock):
    def __init__(self, *args, **kw):
        kw["spec_set"] = ["rollback", "commit"]
        mock.Mock.__init__(self, *args, **kw)

    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        if exc_type:
            self.rollback()
        else:
            self.commit()

class TestDatabaseStorage(object):
    def setup(self):
        self.backend = mock.Mock(spec=backend.SqlAlchemyBackend)
        self.metadata = mock.Mock(spec=oh5.Metadata)
        self.conn = MockConnection()
        self.tx = MockTransaction()
        self.importer = mock.Mock(spec=storage.DatabaseFileImporter)
        self.storage = storage.DatabaseStorage()

        self.backend.get_connection.return_value = self.conn
        self.conn.begin.return_value = self.tx
        self.storage.get_file_importer = mock.Mock(return_value=self.importer)
    
    def test_store(self):
        self.importer.store.return_value = mock.sentinel.oid

        self.storage.store(self.backend, self.metadata, "/path/to/file")
        
        self.backend.get_connection.assert_called_once_with()
        self.conn.begin.assert_called_once_with()
        self.storage.get_file_importer.assert_called_once_with(self.backend)
        self.importer.store.assert_called_with(self.conn, "/path/to/file")
        self.backend.insert_metadata.assert_called_one_with(
            self.conn, self.metadata
        )
        self.conn.execute.assert_called_once_with(
            check_instance(sql.expression.Update),
            oid=mock.sentinel.oid
        )
        self.tx.commit.assert_called_once_with()
        self.conn.close.assert_called_once_with()
    
    def test_store_importer_failure(self):
        self.importer.store.side_effect = Exception()

        try:
            self.storage.store(self.backend, self.metadata, "/path/to/file")
        except Exception:
            pass
        else:
            ok_(False, "expected Exception to be propagated")

        self.backend.get_connection.assert_called_once_with()
        self.conn.begin.assert_called_once_with()
        self.importer.store.assert_called_with(self.conn, "/path/to/file")
        self.backend.insert_metadata.assert_called_one_with(
            self.conn, self.metadata
        )
        eq_(0, self.conn.execute.call_count)
        self.tx.rollback.assert_called_once_with()
        self.conn.close.assert_called_once_with()

    def test_read(self):
        self.importer.read.return_value = mock.sentinel.content
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")
        self.storage.get_oid = mock.Mock(return_value=mock.sentinel.oid)
        
        result = self.storage.read(self.backend, uuid_)
        self.backend.get_connection.assert_called_once_with()
        self.storage.get_oid.assert_called_once_with(self.conn, uuid_)
        self.storage.get_file_importer.assert_called_once_with(self.backend)
        self.conn.execute(check_instance(sql.expression.Select))
        self.importer.read.assert_called_with(self.conn, mock.sentinel.oid)
        self.conn.close.assert_called_once_with()
        eq_(mock.sentinel.content, result)
    
    @raises(storage.FileNotFound)
    def test_read_no_oid(self):
        self.storage.get_oid = mock.Mock(return_value=None)
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")

        self.storage.read(self.backend, uuid_)
   
    def test_remove(self):
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")
        self.storage.get_oid = mock.Mock(return_value=mock.sentinel.oid)

        self.storage.remove(self.backend, uuid_)
        self.backend.get_connection.assert_called_once_with()
        self.storage.get_oid.assert_called_once_with(self.conn, uuid_)
        self.storage.get_file_importer.assert_called_once_with(self.backend)
        self.conn.begin.assert_called_once_with()
        self.importer.remove.assert_called_with(self.conn, mock.sentinel.oid)
        self.backend.delete_metadata.assert_called_once_with(self.conn, uuid_)
        self.tx.commit.assert_called_once_with()
        self.conn.close.assert_called_once_with()
    
    @raises(storage.FileNotFound)
    def test_remove_no_oid(self):
        self.storage.get_oid = mock.Mock(return_value=None)
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")

        self.storage.remove(self.backend, uuid_)
    
    def test_get_oid(self):
        result = mock.Mock(spec_set=["scalar"])
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")
        result.scalar.return_value = mock.sentinel.oid
        self.conn.execute.return_value = result

        eq_(mock.sentinel.oid, self.storage.get_oid(self.conn, uuid_))
        self.conn.execute.assert_called_with(
            check_instance(sql.expression.Select)
        )

class TestFileSystemStorage(object):
    def setup(self):
        self.backend = mock.Mock(spec=backend.SqlAlchemyBackend)
        self.metadata = mock.Mock(spec=oh5.Metadata)
        self.metadata.bdb_uuid = uuid.UUID("abc00000-0000-0000-0004-000000000001")
        self.conn = MockConnection()
        self.tx = MockTransaction()
        self.storage = storage.FileSystemStorage("/stor", layers=1)
        self.storage.ensure_dir_exists = mock.Mock()

        self.backend.get_connection.return_value = self.conn
        self.conn.begin.return_value = self.tx
    
    @mock.patch("shutil.copyfile")
    def test_store(self, copyfile):
        self.storage.store(self.backend, self.metadata, "/infile")
        self.storage.assert_path = mock.Mock()
        
        self.backend.get_connection.assert_called_once_with()
        self.conn.begin.assert_called_once_with()
        self.backend.insert_metadata.assert_called_one_with(
            self.conn, self.metadata
        )
        self.storage.ensure_dir_exists.called_once_with("/stor/a/")
        copyfile.assert_called_with(
            "/infile", "/stor/a/abc00000-0000-0000-0004-000000000001"
        )
        self.tx.commit.assert_called_once_with()
        self.conn.close.assert_called_once_with()
    
    @mock.patch("shutil.copyfile")
    def test_store_copy_failure(self, copyfile):
        copyfile.side_effect = Exception()
        
        try:
            self.storage.store(self.backend, self.metadata, "/infile")
        except Exception:
            pass
        else:
            ok_(False, "expect Exception to propagate")
        
        self.backend.get_connection.assert_called_once_with()
        self.conn.begin.assert_called_once_with()
        self.backend.insert_metadata.assert_called_one_with(
            self.conn, self.metadata
        )
        self.storage.ensure_dir_exists.called_once_with("/stor/a/")
        copyfile.assert_called_with(
            "/infile", "/stor/a/abc00000-0000-0000-0004-000000000001"
        )
        self.tx.rollback.assert_called_once_with()
        self.conn.close.assert_called_once_with()
    
    @mock.patch("__builtin__.open")
    def test_read(self, mock_open):
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")

        mock_open.return_value = mock.MagicMock(spec=file)
        file_handle = mock_open.return_value.__enter__.return_value
        file_handle.read.return_value = "content"

        result = self.storage.read(self.backend, uuid_)

        mock_open.assert_called_once_with(
            "/stor/0/00000000-0000-0000-0004-000000000001"
        )
        file_handle.read.assert_called_once_with()
        eq_("content", result)
    
    @raises(storage.FileNotFound)
    @mock.patch("__builtin__.open")
    def test_read_nx(self, mock_open):
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")
        mock_open.side_effect = IOError(errno.ENOENT, "msg", "filename")

        self.storage.read(self.backend, uuid_)
    
    @raises(IOError)
    @mock.patch("__builtin__.open")
    def test_read_other_exceptions(self, mock_open):
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")
        mock_open.side_effect = IOError(errno.EPERM, "msg", "filename")

        self.storage.read(self.backend, uuid_)

    @mock.patch("os.unlink")
    def test_remove(self, unlink):
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")
        
        self.storage.remove(self.backend, uuid_)
        unlink.assert_called_once_with(
            "/stor/0/00000000-0000-0000-0004-000000000001"
        )
        self.backend.delete_metadata.assert_called_once_with(self.conn, uuid_)
    
    @raises(storage.FileNotFound)
    @mock.patch("os.unlink")
    def test_remove_nx_file(self, unlink):
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")
        unlink.side_effect = OSError(errno.ENOENT, "msg", "filename")
        
        self.storage.remove(self.backend, uuid_)
    
    @raises(OSError)
    @mock.patch("os.unlink")
    def test_remove_other_exceptions(self, unlink):
        uuid_ = uuid.UUID("00000000-0000-0000-0004-000000000001")
        unlink.side_effect = OSError(errno.EPERM, "msg", "filename")
        
        self.storage.remove(self.backend, uuid_)

    def test_path_from_uuid(self):
        strg = storage.FileSystemStorage("/stor", layers=3)
        expected = "/stor/a/b/c/abc00000-0000-0000-0004-000000000001"
        result = strg.path_from_uuid("abc00000-0000-0000-0004-000000000001")
        eq_(expected, result)
    
    def test_from_conf(self):
        # store original method, we are going to patch the class
        from_conf = self.storage.from_conf
        conf = config.Properties({
            "baltrad.bdb.server.backend.sqla.storage.fs.path": "/root",
            "baltrad.bdb.server.backend.sqla.storage.fs.layers": 2,
        })

        with mock.patch(
            "baltrad.bdbserver.sqla.storage.FileSystemStorage"
        ) as ctor:
            ctor.return_value = mock.sentinel.storage
            result = from_conf(conf)
            ctor.assert_called_once_with("/root", layers=2)
            eq_(mock.sentinel.storage, result)
    
    def test_from_conf_default_layers(self):
        # store original method, we are going to patch the class
        from_conf = self.storage.from_conf
        conf = config.Properties({
            "baltrad.bdb.server.backend.sqla.storage.fs.path": "/root",
        })

        with mock.patch(
            "baltrad.bdbserver.sqla.storage.FileSystemStorage"
        ) as ctor:
            ctor.return_value = mock.sentinel.storage
            result = from_conf(conf)
            ctor.assert_called_once_with("/root", layers=3)
            eq_(mock.sentinel.storage, result)
    
    @raises(config.Error)
    def test_from_conf_relative_path(self):
        conf = config.Properties({
            "baltrad.bdb.server.backend.sqla.storage.fs.path": "relpath",
        })
        storage.FileSystemStorage.from_conf(conf)

class TestFileStorage(object):
    @mock.patch("baltrad.bdbserver.sqla.storage.FileStorage.get_impl")
    def test_impl_from_conf(self, get_impl):
        impl_cls = get_impl.return_value
        impl_cls.from_conf.return_value = mock.sentinel.storage

        result = storage.FileStorage.impl_from_conf("mock", mock.sentinel.conf)
        get_impl.assert_called_once_with("mock")
        impl_cls.from_conf.assert_called_once_with(mock.sentinel.conf)
        eq_(mock.sentinel.storage, result)
    
    @raises(config.Error)
    @mock.patch("baltrad.bdbserver.sqla.storage.FileStorage.get_impl")
    def test_impl_from_conf_nx_impl(self, get_impl):
        get_impl.side_effect = LookupError

        storage.FileStorage.impl_from_conf("mock", mock.sentinel.conf)
