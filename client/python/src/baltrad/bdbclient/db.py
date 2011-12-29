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

class DatabaseError(RuntimeError):
    """Exception thrown for failed database requests
    """
    pass

class Database(object):
    """Database interface
    """

    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def store(self, data):
        """store a file in the database

        :param data: a file-like object providing access to the file data
        :return: a FileEntry object for the stored file
        :raise: :class:`DatabaseError` if an error occurs
        """
        raise NotImplementedError()
    
    @abc.abstractmethod
    def get_file_entry(self, uuid):
        """get a file entry in the database

        :param uuid: the uuid of the entry to fetch
        :return: a :class:`~.db.FileEntry` instance or `None` if not found
        """
        raise NotImplementedError()
    
    @abc.abstractmethod
    def get_file_content(self, uuid):
        """get file content from the database

        :param uuid: the uuid of the entry whose content to fetch
        :return: a file-like object with the file content or `None` if not
                 found
        """
        raise NotImplementedError()
    
    @abc.abstractmethod
    def remove_file_entry(self, uuid):
        """remove an entry from the database

        :param uuid: the uuid of the entry to be removed
        :return: `True` if the entry was found and removed, `False` if the
                 entry was not found
        :raise: :class:`DatabaseError` if an error occurs
        """
        raise NotImplementedError()

    @abc.abstractmethod
    def get_sources(self):
        """get all sources defined in this database

        :return: list of :class:`~.bdbcommon.oh5.Source` instances
        """
        raise NotImplementedError()
    
    @abc.abstractmethod
    def add_source(self):
        """add a source to the database
        """

class FileEntry(object):
    __metaclass__ = abc.ABCMeta
    
    def get_uuid(self):
        pass
    
    uuid = abc.abstractproperty(get_uuid)

    def get_metadata(self):
        pass
    
    metadata = abc.abstractproperty(get_metadata)
