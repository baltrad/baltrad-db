/* --------------------------------------------------------------------
Copyright (C) 2009-2012 Swedish Meteorological and Hydrological Institute, SMHI,

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
------------------------------------------------------------------------*/
package eu.baltrad.bdb.storage;

import java.io.File;
import java.util.UUID;

/**
 * Thread safe file cache.
 * @author anders
 */
class FileCache {
  /**
   * The linked file cache.
   */
  private LinkedFileCache cache = null;
  
  /**
   * Constructor
   * @param fileLimit number of files in cache
   */
  public FileCache(int fileLimit) {
    cache = new LinkedFileCache(fileLimit);
  }

  /**
   * Constructor for allowing testing
   * @param cache the cache
   */
  public FileCache(LinkedFileCache cache) {
    this.cache = cache;
  }
  
  /**
   * Puts a file into the cache
   * @param uuid the uuid
   * @param file the file
   */
  public synchronized void put(UUID uuid, File file) {
    cache.put(uuid, file);
  }
  
  /**
   * Returns the file with specified uuid
   * @param uuid the uuid
   * @return the file
   */
  public File get(UUID uuid) {
    return cache.get(uuid);
  }
  
  /**
   * Removes the file with uuid
   * @param uuid the uuid
   */
  public synchronized void remove(UUID uuid) {
    cache.remove(uuid);
  }
}
