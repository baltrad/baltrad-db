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

class DuplicateEntry(Exception):
    """thrown to indicate that an entry already exists
    """

class Backend(object):
    """Backend interface
    """

    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def store_file(self, path):
        """store a file in the database

        :param path: path to the file
        :type path: string
        :return: metadata extracted from the file
        :rtype: :class:`baltrad.bdb.oh5.Metadata`
        :raise: :class:`baltrad.bdb.backend.DuplicateEntry` if file is already stored
        """
        raise NotImplemented()
    
    @abc.abstractmethod
    def get_file(self, uuid):
        """get a file from the database

        :param uuid: UUID the file was stored with
        :type uuid: string
        :return: file content as a string or `None` if file not found
        :rtype: string
        """
        raise NotImplemented()

    @abc.abstractmethod
    def get_file_metadata(self, uuid):
        """get file metadata from the database

        :param uuid: UUID the file was stored with
        :type uuid: string
        :return: :class:`baltrad.bdb.oh5.Metadata` instance
                 or `None` if file not found
        """
        raise NotImplemented()
    
    @abc.abstractmethod
    def remove_file(self, uuid):
        """remove a file stored in the database

        :param uuid: UUID the file was stored with
        :type uuid: string
        :return: `True` if the file was removed or `False` if it didn't exist
        """
        raise NotImplemented()

def create_from_config(config):
    from .sqla.backend import SqlAlchemyBackend
    if config["type"] == "sqlalchemy":
        return SqlAlchemyBackend.create_from_config(config)
