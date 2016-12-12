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

import java.util.List;

public interface FileResult {
  /**
   * Move to next row
   * @return true if there was another result
   */
  boolean next();
  
  int size();

  /**
   * Get the file entry on this row
   */
  FileEntry getFileEntry();
  
  /**
   * Returns the source name for the file on this row. 
   * If no source name is found, null is returned.
   */
  String getSourceName();
  
  /**
   * Returns all entries of the file result.
   */
  List<FileResultEntry> getAllFileResultEntries();

  /**
   * close the result
   */
  void close();
}
