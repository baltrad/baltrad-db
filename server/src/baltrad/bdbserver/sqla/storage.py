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

import abc
import errno
import os
import shutil

import pkg_resources

from sqlalchemy import sql
from tempfile import NamedTemporaryFile

from baltrad.bdbcommon import util
from baltrad.bdbserver import config
from baltrad.bdbserver.sqla import schema

class Error(Exception):
    pass

class FileNotFound(Exception):
    pass

class FileStorage(object):
    """interface for storing physical files on
    :class:`~.sqla.backend.SqlAlchemyBackend`
    """
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def store(self, backend, metadata, content):
        """store a file on the backend

        :param backend: :class:`~sqla.backend.SqlAlchemyBackend` to work for
        :param metadata: :class:`~.oh5.meta.Metadata` extracted from the file
        :param content: the file content. if `string`, implementing
                        classes should treat this as a path to the file
                        location and read the file content from there.
                        Otherwise, it should be treatead as a readable
                        `file-like object` that provides access to the file
                        content.
        :type content: `string` or `file-like object`
        :return: the UUID of the created file entry

        .. note::

          this method has to store both the metadata and file content! This
          can be easily achieved by calling
          :meth:`~.sqla.backend.SqlAlchemyBackend.store_metadata` on the
          backend. This enables storing within one transaction.
        """
        raise NotImplementedError()
    
    @abc.abstractmethod
    def store_file(self, backend, uuid, obj):
        """store a file on the backend

        :param backend: :class:`~sqla.backend.SqlAlchemyBackend` to work for
        :param uuid: :class:`~.oh5.meta.Metadata` extracted from the file
        :param obj: the file content.

        .. note::

          this method has to store both the metadata and file content! This
          can be easily achieved by calling
          :meth:`~.sqla.backend.SqlAlchemyBackend.store_metadata` on the
          backend. This enables storing within one transaction.
        """
        raise NotImplementedError()
        
    @abc.abstractmethod
    def remove(self, backend, uuid):
        """remove a stored filed

        :param backend: :class:`~sqla.backend.SqlAlchemyBackend` to work for
        :param uuid: :class:`~uuid.UUID` of the file to remove
        :raise: :class:`FileNotFound` if the file is not stored

        .. note::

          this method has to remove both the metadata and file content! This
          can be easily achieved by calling
          :meth:`~.sqla.backend.SqlAlchemyBackend.delete_metadata` on the
          backend. This enables removing within one transaction.
        """
        raise NotImplementedError()

    @abc.abstractmethod
    def remove_file(self, backend, uuid):
        """only removes the file without removing the metadata. Used when migrating between different storage types.

        :param backend: :class:`~sqla.backend.SqlAlchemyBackend` to work for
        :param uuid: :class:`~uuid.UUID` of the file to remove
        :raise: :class:`FileNotFound` if the file is not stored

        .. note::

          this method has to ensure that the reference to the file content is removed from the affected tables.
        """
        raise NotImplementedError()
    
    @abc.abstractmethod
    def read(self, backend, uuid):
        """read the content of a stored file

        :param backend: :class:`~sqla.backend.SqlAlchemyBackend` to work for
        :param uuid: :class:`~uuid.UUID` of the file to remove
        :raise: :class:`FileNotFound` if the file is not stored
        :return: file content as a string
        """
        raise NotImplementedError()
    
    @util.abstractclassmethod
    def from_conf(cls, conf):
        raise NotImplementedError()
    
    @classmethod
    def get_impl(cls, name):
        """get an implementing class by name

        the implementing class is looked up from
        'baltrad.bdbserver.sqla.storage' entry point. 

        :raise: :class:`LookupError` if not found
        """
        try:
            return pkg_resources.load_entry_point(
                "baltrad.bdbserver",
                "baltrad.bdbserver.sqla.storage",
                name
            )
        except ImportError:
            raise LookupError(name)
    
    @classmethod
    def impl_from_conf(cls, name, conf):
        try:
            impl_cls = cls.get_impl(name)
        except LookupError:
            raise config.Error("unsupported storage type: %s" % name)
        return impl_cls.from_conf(conf)

class DatabaseFileImporter(object):
    __meta__ = abc.ABCMeta
    
    @abc.abstractmethod
    def store(self, conn, path):
        raise NotImplementedError()
    
    @abc.abstractmethod
    def read(self, conn, oid):
        raise NotImplementedError()
    
    
    @abc.abstractmethod
    def remove(self, conn, oid):
        raise NotImplementedError()

    @abc.abstractmethod
    def remove_file(self, conn, oid):
        raise NotImplementedError()
    
class GenericDatabaseFileImporter(DatabaseFileImporter):
    def store(self, conn, path):
        return conn.execute(
            schema.file_data.insert(),
            data=open(path, "r").read()
        ).inserted_primary_key[0]
    
    def read(self, conn, oid):
        return conn.execute(
            sql.select(
                [schema.file_data.c.data],
                schema.file_data.c.id==oid
            )
        ).scalar()
    
    def remove(self, conn, oid):
        conn.execute(
            schema.file_data.delete().where(schema.file_data.c.id==oid)
        )

    def remove_file(self, conn, oid):
        self.remove(conn, oid)
        
class Psycopg2DatabaseFileImporter(DatabaseFileImporter):
    def store(self, conn, path):
        lobj = conn.connection.lobject(new_file=path)
        return lobj.oid

    def read(self, conn, oid):
        lobj = conn.connection.lobject(oid=oid, mode="r")
        return lobj.read()
    
    def remove(self, conn, oid):
        #lobj = conn.connection.lobject(oid=oid)
        #lobj.unlink()
        pass

    def remove_file(self, conn, oid):
        lobj = conn.connection.lobject(oid=oid)
        lobj.unlink()

class DatabaseStorage(FileStorage):
    """:class:`~FileStorage` storing the files in the database.

    If the engine backend is using *psycopg2* driver, the files are stored as
    large objects. In all other cases, a generic approach is used and files
    are stored as blobs in :attr:`~.schema.bdb_file_data`.

    column `lo_id` in :attr:`~.schema.bdb_files` is used to store both the
    postgresql large object id and the id from :attr:`~.schema.bdb_file_data`
    in the generic case.
    """
    
    def store(self, backend, metadata, path):
        with backend.get_connection() as conn:
            with conn.begin():
                file_id = backend.insert_metadata(conn, metadata)
                importer = self.get_file_importer(backend)

                oid = importer.store(conn, path)
                conn.execute(
                    schema.files.update().where(schema.files.c.id==file_id),
                    oid=oid
                )

    def store_file(self, backend, uuid, obj):
        with backend.get_connection() as conn:
            with conn.begin():        
                with NamedTemporaryFile() as tmp:
                    tmp.write(obj)
                    tmp.flush()
                    importer = self.get_file_importer(backend)
                    oid = importer.store(conn, tmp.name)
                    conn.execute(schema.files.update().where(schema.files.c.uuid==uuid),
                                 oid=oid)

        
    def read(self, backend, uuid):
        with backend.get_connection() as conn:
            oid = self.get_oid(conn, uuid)
            if oid:
                return self.get_file_importer(backend).read(conn, oid)
            else:
                raise FileNotFound("no oid mapped for %s" % uuid)

    def remove(self, backend, uuid):
        with backend.get_connection() as conn:
            oid = self.get_oid(conn, uuid)
            if oid:
                with conn.begin():
                    self.get_file_importer(backend).remove(conn, oid)
                    backend.delete_metadata(conn, uuid)
            else:
                raise FileNotFound("no oid mapped for %s" % uuid)
    
    def remove_file(self, backend, uuid):
        with backend.get_connection() as conn:
            oid = self.get_oid(conn, uuid)
            if oid:
                with conn.begin():
                    self.get_file_importer(backend).remove_file(conn, oid)
                    conn.execute(schema.files.update().where(schema.files.c.uuid==uuid), oid=None)
            else:
                raise FileNotFound("no oid mapped for %s" % uuid)

    def get_file_importer(self, backend):
        if backend.driver == "psycopg2":
            return Psycopg2DatabaseFileImporter()
        else:
            return GenericDatabaseFileImporter()
    
    def get_oid(self, conn, uuid):
        qry = sql.select(
            [schema.files.c.oid],
            schema.files.c.uuid==str(uuid)
        )
        return conn.execute(qry).scalar()
        
    
    @classmethod
    def from_conf(cls, conf):
        return DatabaseStorage()

class FileSystemStorage(FileStorage):
    """:class:`~FileStorage` storing the files in the filesystem

    :param path: the location to store the files to
    :param layers: number of directory layers before actually storing the
                   file.

    The layers are created from the file uuid that is best explained by an
    example. For *path*=/storage/ and *layers*=3, a file with *uuid*=abcdef
    would be stored as */storage/a/b/c/abcdef.h5*.
    """
    def __init__(self, path, layers=3):
        self._path = path
        self._layer_count = layers

    def store(self, backend, metadata, path):
        with backend.get_connection() as conn:
            with conn.begin():
                backend.insert_metadata(conn, metadata)
                target = self.path_from_uuid(metadata.bdb_uuid)
                self.ensure_dir_exists(os.path.dirname(target))
                shutil.copyfile(path, target)
    
    def store_file(self, backend, uuid, obj):
        target = self.path_from_uuid(uuid)
        self.ensure_dir_exists(os.path.dirname(target))
        with open(target, "w") as fp:
            fp.write(obj)
    
    def read(self, backend, uuid):
        target = self.path_from_uuid(uuid)
        try:
            with open(target) as f:
                return f.read()
        except IOError, e:
            if e.errno == errno.ENOENT:
                raise FileNotFound(target)
            raise

    def remove(self, backend, uuid):
        target = self.path_from_uuid(uuid)
        with backend.get_connection() as conn:
            try:
                os.unlink(target)
            except OSError, e:
                if e.errno == errno.ENOENT:
                    raise FileNotFound(target)
                raise
            backend.delete_metadata(conn, uuid)

    def remove_file(self, backend, uuid):
        target = self.path_from_uuid(uuid)
        try:
            os.unlink(target)
        except OSError, e:
            if e.errno == errno.ENOENT:
                raise FileNotFound(target)
            raise

    def path_from_uuid(self, uuid):
        uuid_str = str(uuid)
        elements = [self._path]
        for i in range(0, self._layer_count):
            elements.append(uuid_str[i])
        elements.append(uuid_str)

        return os.path.join(*elements)
    
    def ensure_dir_exists(self, target):
        if not os.path.exists(target):
            os.makedirs(target)
    
    @classmethod
    def from_conf(cls, conf):
        """create an instance from configuration

        :param conf: a :class:`~.config.Properties` instance to configure
                     from
        :raise: :class:`config.Error` when erroneous configuration is supplied
        :return: an instance of this class

        all configuration attributes are looked up from under
        *'baltrad.bdb.server.backend.sqla.storage.fs.'*.
        """
        conf = conf.filter("baltrad.bdb.server.backend.sqla.storage.fs.")
        path = conf.get("path")
        if not os.path.isabs(path):
            raise config.Error("%s must be absolute: %s" % (
                conf.get_full_key("path"), path
            ))
        layers = conf.get_int("layers", 3)
        return FileSystemStorage(path, layers=layers)
