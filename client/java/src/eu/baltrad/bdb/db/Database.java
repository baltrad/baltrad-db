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
package eu.baltrad.bdb.db;

import eu.baltrad.bdb.oh5.Source;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.List;
import java.util.UUID;

public interface Database {
  /**
   * Store file in the database.
   *
   * @param fileContent file content stream
   * @return a FileEntry instance for the saved file
   * @throws DatabaseError if a database error occurs
   * @throws DuplicateEntry when file already stored in the database
   */
  FileEntry store(InputStream fileContent);

  /**
   * Remove file entry from the database.
   *
   * @param uuid the UUID of entry to remove
   * @return true if the file was removed, false if it did not exist
   * @throws DatabaseError if a database error occurs
   */
  boolean removeFileEntry(UUID uuid);

  /**
   * Remove all file entries from the database.
   */
  void removeAllFileEntries();
  
  /**
   * get FileEntry by UUID
   * @param uuid the UUID of the entry
   * @return null if not found
   * @throws DatabaseError if a database error occurs
   */
  FileEntry getFileEntry(UUID uuid);
  
  /**
   * @return the number of files stored in the database
   * @throws DatabaseError if a database error occurs
   */
  long getFileCount();
  
  /**
   * get File content from the database
   * @param uuid the UUID of of the entry
   * @return stream to the file content or null if not found
   */
  InputStream getFileContent(UUID uuid);

  /**
   * execute a query
   */
  FileResult execute(FileQuery query);
  
  /**
   * execute a query
   */
  AttributeResult execute(AttributeQuery query);

  /**
   * release any acquired resources
   */
  void close();
  
  /**
   * list sources defined in the database
   */
  @Deprecated
  List<Source> getSources();
  
  /**
   * @return the source manager
   */
  SourceManager getSourceManager();
}
