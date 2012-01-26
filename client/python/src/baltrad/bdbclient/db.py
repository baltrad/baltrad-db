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
import json

from baltrad.bdbcommon import expr

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
    def execute_file_query(self, query):
        """execute a file query

        :param query: a :class:`FileQuery` to execute
        :return: a `FileResult` instance
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
        raise NotImplementedError()
    
    @abc.abstractmethod
    def update_source(self, name, source):
        """update a source in the database
        """
        raise NotImplementedError()
    
    @abc.abstractmethod
    def remove_source(self, name):
        """remove a source from the database
        """
        raise NotImplementedError()

class FileEntry(object):
    __metaclass__ = abc.ABCMeta
    
    def get_uuid(self):
        pass
    
    uuid = abc.abstractproperty(get_uuid)

    def get_metadata(self):
        pass
    
    metadata = abc.abstractproperty(get_metadata)

class FileQuery(object):
    def __init__(self):
        self.filter = None
        self.order = []
        self.limit = None
        self.skip = None
    
    def to_json(self):
        json_repr = {}
        if self.filter:
            json_repr["filter"] = expr.wrap_json(self.filter)
        json_repr["order"] = [expr.wrap_json(o) for o in self.order]
        if self.limit:
            json_repr["limit"] = self.limit
        if self.skip:
            json_repr["skip"] = self.skip
        return json.dumps(json_repr)

class FileResult(object):
    """The result of executing a :class:`FileQuery`

    intended usage::

        >>> while result.next():
        ...     entry = result.get_file_entry()
    """
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def next(self):
        """move to next :class:`FileEntry`, return `True` if successful
        """
        raise NotImplementedError()
    
    @abc.abstractmethod
    def size(self):
        """return the size of the result set
        """
        raise NotImplementedError()
    
    @abc.abstractmethod
    def get_file_entry(self):
        """return current :class:`FileEntry`

        :raise: `LookupError` when exhausted
        """
        raise NotImplementedError()
    
    @abc.abstractmethod
    def get_uuid(self):
        """return the UUID of current :class:`FileEntry`

        :raise: `LookupError` when exhausted

        this *might* be more efficiently implemented the getting the whole
        entry and the uuid from there.
        """
        raise NotImplementedError()
