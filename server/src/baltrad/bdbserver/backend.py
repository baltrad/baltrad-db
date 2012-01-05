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

from abc import abstractmethod, ABCMeta

import pkg_resources

from baltrad.bdbcommon.util import abstractclassmethod

class DuplicateEntry(Exception):
    """thrown to indicate that an entry already exists
    """

class IntegrityError(RuntimeError):
    """thrown to indicate a conflict between resources
    """
    pass

class Backend(object):
    """Backend interface
    """

    __metaclass__ = ABCMeta

    @abstractmethod
    def store_file(self, path):
        """store a file in the database

        :param path: path to the file
        :type path: string
        :return: metadata extracted from the file
        :rtype: :class:`baltrad.bdb.oh5.meta.Metadata`
        :raise: :class:`baltrad.bdb.backend.DuplicateEntry` if file is already stored
        """
        raise NotImplementedError()
    
    @abstractmethod
    def get_file(self, uuid):
        """get a file from the database

        :param uuid: UUID the file was stored with
        :type uuid: :class:`uuid.UUID`
        :return: file content as a string or `None` if file not found
        :rtype: string
        """
        raise NotImplementedError()

    @abstractmethod
    def get_file_metadata(self, uuid):
        """get file metadata from the database

        :param uuid: UUID the file was stored with
        :type uuid: :class:`uuid.UUID`
        :return: :class:`baltrad.bdb.oh5.meta.Metadata` instance
                 or `None` if file not found
        """
        raise NotImplementedError()
    
    @abstractmethod
    def remove_file(self, uuid):
        """remove a file stored in the database

        :param uuid: UUID the file was stored with
        :type uuid: :class:`uuid.UUID`
        :return: `True` if the file was removed or `False` if it didn't exist
        """
        raise NotImplementedError()

    @abstractmethod
    def remove_all_files(self, query):
        """remove all files from the database
        """
        raise NotImplementedError()
    
    @abstractmethod
    def get_sources(self):
        """get a list of sources defined in the database

        :return: list of :class:`baltrad.bdb.oh5.meta.Source` instances
        """
        raise NotImplementedError()
    
    @abstractmethod
    def add_source(self, source):
        """add a new source definition to the database

        :param source: the source definition to add
        :type source: :class:`baltrad.bdb.oh5.meta.Source`
        :raise: :class:`DuplicateEntry` if this source is already defined
        """
        raise NotImplementedError()
    
    @abstractmethod
    def remove_source(self, name):
        """remove a source definition from the database

        :param name: the name of the source
        :return: `True` if the source was remove, `False` if not found
        :raise: :class:`IntegrityError` if the source is associated with files
        """
        raise NotImplementedError()
    
    @abstractmethod
    def update_source(self, name, source):
        """update a source definition in the database

        :param name: the name of the source to modify
        :param source: the new definiton of the source
        :raise: :class:`DuplicateEntry` if renaming and a source with such a
                name already exists
        :raise: :class:`LookupError` if the source doesn't exist
        """
        raise NotImplementedError()
    
    @abstractmethod
    def execute_file_query(self, query):
        """execute a file query
        
        :param query: the query to execute
        :type query: :class:`FileQuery` instance
        """
        raise NotImplementedError()
    
    @abstractmethod
    def execute_attribute_query(self, query):
        """execute an attribute query
        
        :param query: the query to execute
        :type query: :class:`AttributeQuery` instance
        """
        raise NotImplementedError()
    
    @abstractmethod
    def is_operational(self):
        """test if the backend is fully operational
        """
        raise NotImplementedError()

    @abstractmethod
    def create(self):
        """create resources on the backend
        """
        raise NotImplementedError()

    @abstractmethod
    def drop(self):
        """drop resources on the backend
        """
        raise NotImplementedError()    
    
    @abstractmethod
    def upgrade(self):
        """bring the resources up-to-date on backend
        """
        raise NotImplementedError()
    
    @abstractclassmethod
    def create_from_config(cls, config):
        """create an instance and configure it
        """
    
    @classmethod
    def get_implementation_by_name(cls, name):
        """get an implementing class by name

        the implementing class is looked up from 'baltrad.bdbserver.backends'
        entry point. 

        :raise: :class:`LookupError` if not found
        """
        try:
            return pkg_resources.load_entry_point(
                "baltrad.bdbserver",
                "baltrad.bdbserver.backends",
                name
            )
        except ImportError:
            raise LookupError(name)
    
def create_from_config(config):
    backend_type = config["backend.type"]
    if backend_type == "sqlalchemy":
        from .sqla.backend import SqlAlchemyBackend
        return SqlAlchemyBackend.create_from_config(config)
    else:
        raise ValueError("unsupported backend type: " + backend_type)

class FileQuery(object):
    def __init__(self):
        self.filter = None
        self.order = []
        self.limit = None
        self.skip = None
    
    def execute(self, backend):
        return backend.execute_file_query(self)

class AttributeQuery(object):
    def __init__(self):
        self.fetch = {}
        self.filter = None
        self.distinct = False
        self.order = []
        self.group = []
        self.limit = None
        self.skip = None
        
    def execute(self, backend):
        return backend.execute_attribute_query(self)
