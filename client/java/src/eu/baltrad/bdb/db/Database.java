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

import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Source;
import eu.baltrad.bdb.util.DateTime;

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
   * Queries a file for its metadata without storing it in database
   *
   * @param fileContent file content stream
   * @return a FileEntry instance for the saved file
   * @throws DatabaseError if a database error occurs
   */
  Metadata queryFileMetadata(InputStream fileContent);
  
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
   * Removes a number of files from the database. limit defines the minimum number of files in the database
   * and nritems is the maximum number of items to be removed. This means that the number of files removed will
   * be either nritems or (number of files in db) - limit depending on which is the smallest.
   * 
   * @param limit minimum number of files in database
   * @param nritems maximum number of files to remove
   * @return the number of files removed
   */
  int removeFilesByCount(int limit, int nritems);

  /**
   * Removes files older than age or up to number of items
   * 
   * @param age age limit
   * @param nritems maximum number of files to remove
   * @return the number of files removed
   */
  int removeFilesByAge(DateTime age, int nritems);
  
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
