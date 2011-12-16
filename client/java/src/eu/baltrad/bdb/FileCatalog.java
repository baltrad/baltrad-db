/*
Copyright 2010-2011 Estonian Meteorological and Hydrological Institute

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
*/

package eu.baltrad.bdb;

import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.db.Database;
import eu.baltrad.bdb.storage.LocalStorage;

import java.io.*;
import java.util.UUID;

public interface FileCatalog {
  /**
   * access the associated database instance.
   */
  Database getDatabase();
  
  /**
   * access the associated storage instance.
   */
  LocalStorage getLocalStorage();
  
  /**
   * store a file in the database and local storage.
   */
  FileEntry store(InputStream fileContent);
  
  /**
   * remove a file from the database, possibly removing it from local storage.
   */
  void remove(FileEntry fileEntry);
  
  /**
   * get local path for a file stored with uuid
   */
  File getLocalPathForUuid(UUID uuid);
  // XXX: getLocalPathForUiid clutters this interface and should be declared
  //      elsewhere (in LocalStorage?)
}
