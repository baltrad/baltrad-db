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
import org.joda.time.LocalDateTime;

public final class DateTime {
  private final LocalDateTime value;
  
  /**
   * Construct using current UTC timestamp
   */
  public DateTime() {
    value = new LocalDateTime(DateTimeZone.UTC);
  }

  public DateTime(int year, int month, int day, int hour, int minute) {
    this(year, month, day, hour, minute, 0, 0);
  }

  public DateTime(int year, int month, int day, int hour, int minute, int second) {
    this(year, month, day, hour, minute, second, 0);
  }

  public DateTime(int year, int month, int day, int hour, int minute, int second, int msec) {
    value = new LocalDateTime(year, month, day, hour, minute, second, msec);
  }

  protected DateTime(LocalDateTime value) {
    this.value = value;
  }

  public DateTime(Date date, Time time) {
    value = new LocalDateTime(
      date.year(), date.month(), date.day(),
      time.hour(), time.minute(), time.second(), time.msec()
    );
  }

  public static DateTime fromIsoString() {
    return null;
  }

  public static DateTime utcNow() {
    return new DateTime();
  }

  public Date date() {
    return new Date(value.toLocalDate());
  }

  public Time time() {
    return new Time(value.toLocalTime());
  }

  public DateTime add(TimeDelta delta) {
    return new DateTime(value.plus(delta.getJodaPeriod()));
  }

  public String toIsoString() {
    return null;
  }

  @Override
  public boolean equals(Object other) {
    if (this == other)
      return true;
    
    if (!(other instanceof DateTime))
      return false;
    
    DateTime that = (DateTime)(other);
    return this.value.equals(that.value);
  }

  @Override
  public int hashCode() {
    return this.value.hashCode();
  }
}
