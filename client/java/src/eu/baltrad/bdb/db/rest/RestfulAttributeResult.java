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

import eu.baltrad.bdb.db.AttributeResult;

import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.NoSuchElementException;


public class RestfulAttributeResult implements AttributeResult {
  List<Map<String, Object>> rows;
  ListIterator<Map<String, Object>> rowIterator;
  Map<String, Object> currentRow;

  public RestfulAttributeResult(List<Map<String, Object>> rows) {
    this.rows = rows;
    this.rowIterator = rows.listIterator();
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

  public int size() {
    return rows.size();
  }

  @Override
  public boolean isNull(String name) {
    return currentRow.get(name) == null;
  }

  @Override
  public String getString(String name) {
    return (String)currentRow.get(name);
  }

  @Override
  public Double getDouble(String name) {
    return (Double)currentRow.get(name);
  }

  @Override
  public Long getLong(String name) {
    return (Long)currentRow.get(name);
  }

  @Override
  public void close() {
    // no-op
  }
}
