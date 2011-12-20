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
import eu.baltrad.bdb.util.Date;
import eu.baltrad.bdb.util.Time;

import java.io.File;
import java.io.InputStream;
import java.util.UUID;

public interface FileEntry {
  /**
   * Access metadata associated with this entry.
   */
  Metadata getMetadata();

  /**
   * Get the unique identifier of this entry.
   */
  UUID getUuid();
    
  /**
   * Get the date this entry was stored at.
   */
  Date getStoredDate();
  
  /**
   * Get the time this entry was stored at.
   */
  Time getStoredTime();

  /**
   * Get the associated source (as defined in the database).
   */
  Source getSource();

  /**
   * Get the size of the content in bytes.
   */
  int getContentSize();

  /**
   * Get a stream to the file content.
   */
  InputStream getContentStream();
  
  /**
   * Write file contents to a file.
   */
  @Deprecated
  void writeToFile(File file);
}
