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

package eu.baltrad.bdb.util;

import org.joda.time.DateTimeZone;
import org.joda.time.LocalDate;

public class Date {
  private final LocalDate value;
  
  /**
   * Construct with current UTC date
   */
  public Date() {
    this.value = new LocalDate(DateTimeZone.UTC);
  }

  public Date(int year, int month, int day) {
    this.value = new LocalDate(year, month, day);
  }

  protected Date(LocalDate value) {
    this.value = value;
  }

  public static Date fromIsoString(String value) {
    return null;
  }

  public int year() { return value.getYear(); }
  public int month() { return value.getMonthOfYear(); }
  public int day() { return value.getDayOfMonth(); }

  public String toIsoString() {
    return null;
  }
}
