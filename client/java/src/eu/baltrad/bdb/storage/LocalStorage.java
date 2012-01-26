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

package eu.baltrad.bdb.storage;

import eu.baltrad.bdb.db.FileEntry;

import java.io.File;
import java.io.InputStream;

/**
 * Client-side physical file storage. The objective of the implementing
 * classes is to pull the physical files from the server and make them
 * accessible in the local filesystem.
 */
public interface LocalStorage {
  /**
   * Write the file to this storage, associating it with the entry.
   *
   * @param entry fileentry the file was associated with
   * @param path absolute path to the file in filesystem
   * @return absolute path to the stored file
   */
  File store(FileEntry entry, InputStream fileContent);

  /**
   * Write the file to this storage from FileEntry content stream. The
   * content of the entry is accessible by opening the the returned file.
   *
   * @return absolute path to the stored file
   */
  File store(FileEntry entry);
  
  /**
   * Remove the file from this storage if it exists.
   */
  void remove(FileEntry entry);
}
