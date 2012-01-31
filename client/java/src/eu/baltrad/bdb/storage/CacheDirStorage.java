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
import org.apache.commons.io.IOUtils;

import java.io.*;
import java.util.*;

/**
 * store a limited amount of files in a local cache.
 */
public class CacheDirStorage implements LocalStorage {
  private static final int DEFAULT_LIMIT = 1000;

  private final File storageRoot;
  private final FileCache cache;

  public CacheDirStorage(File storageRoot) {
    this(storageRoot, DEFAULT_LIMIT);
  }

  public CacheDirStorage(File storageRoot, int fileLimit) {
    this.storageRoot = storageRoot;
    this.cache = new FileCache(fileLimit);
  }

  protected CacheDirStorage(File storageRoot, FileCache cache) {
    this.storageRoot = storageRoot;
    this.cache = cache;
  }

  @Override
  public File store(FileEntry entry, InputStream fileContent) {
    UUID uuid = entry.getUuid();
    File entryPath = cache.get(uuid);
    if (entryPath == null) {
      entryPath = pathForEntry(entry);
      copyInputStreamToFile(fileContent, entryPath);
      cache.put(uuid, entryPath);
    }
    return entryPath;
  }

  @Override
  public File store(FileEntry entry) {
    InputStream contentStream = entry.getContentStream();
    try {
      return store(entry, contentStream);
    } finally {
      IOUtils.closeQuietly(contentStream);
    }
  }

  @Override
  public void remove(FileEntry entry) {
    cache.remove(entry.getUuid());
  }

  protected File pathForEntry(FileEntry entry) {
    return new File(storageRoot, entry.getUuid().toString());
  }

  protected void copyInputStreamToFile(InputStream src, File dst) {
    try {
      FileUtils.copyInputStreamToFile(src, dst);
    } catch (IOException e) {
      throw new RuntimeException("failed to copy InputStream to " + dst, e);
    }
  }
}
