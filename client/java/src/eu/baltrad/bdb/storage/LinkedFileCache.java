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

import java.io.*;
import java.util.*;

class LinkedFileCache extends LinkedHashMap<UUID, File> {
  private static final long serialVersionUID = 1L;

  private final int fileLimit;

  public LinkedFileCache() {
    this(1000);
  }
  
  public LinkedFileCache(int fileLimit) {
    super(fileLimit, 1, true);
    this.fileLimit = fileLimit;
  }

  @Override
  public File remove(Object key) {
    File path = super.remove(key);
    if (path != null)
      deleteFile(path);
    return path;
  }
  
  @Override
  protected boolean removeEldestEntry(Map.Entry<UUID, File> eldest) {
    boolean remove = size() > fileLimit;
    if (remove)
      deleteFile(eldest.getValue());
    return remove;
  }

  protected void deleteFile(File path) {
    if (path != null) {
      path.delete();
    }
  }
};
