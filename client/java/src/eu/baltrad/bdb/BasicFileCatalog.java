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

package eu.baltrad.bdb;

import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.db.Database;
import eu.baltrad.bdb.storage.LocalStorage;
import eu.baltrad.bdb.storage.NullStorage;

import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

import org.apache.commons.io.IOUtils;

import java.io.*;
import java.util.UUID;

public final class BasicFileCatalog implements FileCatalog {
  Database database;
  LocalStorage localStorage;

  private static Logger logger = LogManager.getLogger(FileCatalog.class);

  public BasicFileCatalog(Database database, LocalStorage localStorage) {
    this.database = database;
    this.localStorage = localStorage;
  }

  public BasicFileCatalog(Database database) {
    this(database, new NullStorage());
  }
  
  @Override
  public Database getDatabase() { return database; }

  public void setDatabase(Database database) {
    this.database = database;
  }
  
  @Override
  public LocalStorage getLocalStorage() {
    return localStorage;
  }
  
  @Override
  public FileEntry store(InputStream fileContent) {
    // since the stream is possibly used twice, read it to byte array
    // and create new InputStreams from that.
    // XXX: There might be more efficient ways to do this.

    byte[] fileContentBytes;
    try {
      fileContentBytes = IOUtils.toByteArray(fileContent);
    } catch (IOException e) {
      throw new RuntimeException("failed to read fileContent stream", e);
    }

    InputStream fileContentByteStream = null;
    FileEntry fileEntry = null;
        
    try {
      fileContentByteStream = new ByteArrayInputStream(fileContentBytes);
      fileEntry = database.store(fileContentByteStream);
    } finally {
      IOUtils.closeQuietly(fileContentByteStream);
    }

    fileContentByteStream = new ByteArrayInputStream(fileContentBytes);
    try {
      localStorage.store(fileEntry, fileContentByteStream);
    } catch (Exception e) {
      logger.warn("Ignored exception thrown in LocalStorage::prestore", e);
    } finally {
      IOUtils.closeQuietly(fileContentByteStream);
    }
    
    logger.debug("Stored file in database and in local storage. UUID: " + fileEntry.getUuid());
    
    return fileEntry;
  }
  
  @Override
  public void remove(FileEntry fileEntry) {
    database.removeFileEntry(fileEntry.getUuid());
    try {
      localStorage.remove(fileEntry);
    } catch (Exception e) {
      logger.warn("Ignored exception thrown in LocalStorage::remove", e);
    }
    logger.debug("Removed file from database and local storage. UUID: " + fileEntry.getUuid());
  }
  
  @Override
  public File getLocalPathForUuid(UUID uuid) {
    FileEntry fileEntry = database.getFileEntry(uuid);
    return localStorage.store(fileEntry);
  }
}
