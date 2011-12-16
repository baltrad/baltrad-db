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

import org.apache.commons.io.FileUtils;

import java.io.*;
import java.nio.channels.Channels;

/**
 * store a limited amount of files in a local cache.
 */
public final class CacheDirStorage implements LocalStorage {
  private final File storageRoot;
  private final int fileLimit;

  private static final int DEFAULT_LIMIT = 1000;

  CacheDirStorage(File storageRoot) {
    this(storageRoot, DEFAULT_LIMIT);
  }

  CacheDirStorage(File storageRoot, int fileLimit) {
    this.storageRoot = storageRoot;
    this.fileLimit = fileLimit;
  }

  @Override
  public File store(FileEntry entry, InputStream fileContent) {
    File entryPath = pathForEntry(entry);
    try {
      FileUtils.copyInputStreamToFile(fileContent, entryPath);
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
    return entryPath;
  }

  @Override
  public File store(FileEntry entry) {
    File entryPath = pathForEntry(entry);
    try {
      InputStream fileContent = entry.getContentStream();
      try {
        FileUtils.copyInputStreamToFile(fileContent, entryPath);
      } finally {
        fileContent.close();
      }
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
    return entryPath;
  }

  @Override
  public void remove(FileEntry entry) {
    File entryPath = pathForEntry(entry);
    entryPath.delete();
  }

  protected File pathForEntry(FileEntry entry) {
    return new File(storageRoot, entry.getUuid().toString());
  }
}
