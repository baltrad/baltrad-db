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

import org.apache.commons.lang3.StringUtils;

import java.io.*;
import java.util.*;

/**
 * Access the files from the server's storage.
 *
 * This implementation is usable when the server is configured to store
 * the files in the filesystem and that storage is accessible from the
 * client side.
 */
class ServerFileStorage implements LocalStorage {
  private File path;
  private int layers;
  
  /**
   * Constructor.
   *
   * @param path location of the server's storage directory
   * @param layers number of layers the server's storage has been
   *               configured with
   */
  public ServerFileStorage(File path, int layers) {
    this.path = path;
    this.layers = layers;
  }

  /**
   * Return the path where the server has stored this entry.
   *
   * @see eu.baltrad.bdb.storage.LocalStorage#store(FileEntry, InputStream)
   */
  public File store(FileEntry entry, InputStream fileContent) {
    return getPathForUuid(entry.getUuid());
  }

  /**
   * Return the path where the server has stored this entry.
   *
   * @see eu.baltrad.bdb.storage.LocalStorage#store(FileEntry)
   */
  public File store(FileEntry entry) {
    return getPathForUuid(entry.getUuid());
  }
  
  /**
   * Do nothing and let the server remove the file.
   *
   * @see eu.baltrad.bdb.storage.LocalStorage#remove(FileEntry)
   */
  public void remove(FileEntry entry) {
    
  }

  protected File getPathForUuid(UUID uuid) {
    List<String> elements = new ArrayList<String>();
    elements.add(path.toString());
    String uuidStr = uuid.toString();
    for (int i=0; i < layers; i++) {
      elements.add(uuidStr.substring(i, i+1));
    }
    elements.add(uuidStr);
    return new File(StringUtils.join(elements.iterator(), "/"));
  }
}
