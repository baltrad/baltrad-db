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

package eu.baltrad.bdb.oh5;

import org.apache.commons.lang3.StringUtils;

import java.util.*;

public class Source {
  String name;
  Map<String, String> values;

  public Source() {
    this.values = new HashMap<String, String>();
  }

  public Source(String name) {
    this();
    this.name = name;
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public String put(String key, String value) {
    return this.values.put(key, value);
  }

  public boolean has(String key) {
    return values.containsKey(key);
  }
  
  public String get(String key) {
    return values.get(key);
  }

  public Collection<String> getKeys() {
    return values.keySet();
  }

  public boolean isEmpty() {
    return values.isEmpty();
  }
  
  /**
   * Construct from a string.
   *
   * @throws InvalidSource if source can't be parsed from the string
   */
  public static Source fromString(String source) {
    Source result = new Source();
    String[] elements = StringUtils.split(source, ",");
    for (int i=0; i < elements.length; i++) {
      String[] keyValuePair = StringUtils.split(elements[i], ":");
      if (keyValuePair.length != 2) {
        throw new InvalidSource(source);
      } else {
        result.put(keyValuePair[0], keyValuePair[1]);
      }
    }
    if (source.isEmpty()) {
      throw new InvalidSource();
    }
    return result;
  }
}
