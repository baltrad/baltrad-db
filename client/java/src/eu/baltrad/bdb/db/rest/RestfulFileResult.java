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
package eu.baltrad.bdb.db.rest;

import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.UUID;

import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.db.FileResult;
import eu.baltrad.bdb.db.FileResultEntry;

public class RestfulFileResult implements FileResult {
  RestfulDatabase database;
  List<Map<String, Object>> rows;
  ListIterator<Map<String, Object>> rowIterator;
  Map<String, Object> currentRow;

  public RestfulFileResult(RestfulDatabase database,
                           List<Map<String, Object>> rows) {
    this.database = database;
    this.rows = rows;
    this.rowIterator = this.rows.listIterator();
    this.currentRow = null;
  }

  @Override
  public boolean next() {
    try {
      currentRow = rowIterator.next();
    } catch (NoSuchElementException e) {
      return false;
    }
    return true;
  }

  @Override
  public int size() {
    return rows.size();
  }

  @Override
  public RestfulFileEntry getFileEntry() {
    if (currentRow != null) {
      String uuidString = (String)currentRow.get("uuid");
      UUID uuid = UUID.fromString(uuidString);
      return database.getFileEntry(uuid);
    }
    throw new NoSuchElementException("RestfulFileResult exhausted");
  }
  
  @Override
  public String getSourceName() {
    if (currentRow != null) {
      String sourceName = (String)currentRow.get("source_name");
      return sourceName;
    }
    throw new NoSuchElementException("RestfulFileResult exhausted");
  }  

  @Override
  public List<FileResultEntry> getAllFileResultEntries() {
    List<FileResultEntry> entries = new ArrayList<FileResultEntry>();
    
    for (Map<String, Object> row : rows) {
      String uuidString = (String)row.get("uuid");
      UUID uuid = UUID.fromString(uuidString);
      String sourceName = (String)row.get("source_name");
      entries.add(new RestfulFileResultEntry(uuid, sourceName, database));
    }
    
    return entries;
  }
  
  @Override
  public void close() {
    // no-op
  }


}
