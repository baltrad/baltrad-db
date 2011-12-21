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
import org.joda.time.LocalTime;
import org.joda.time.format.DateTimeFormatter;
import org.joda.time.format.DateTimeFormatterBuilder;

public final class Time {
  private static DateTimeFormatter isoFormatter;
  private static DateTimeFormatter extendedIsoFormatter;

  static {
    DateTimeFormatterBuilder builder = new DateTimeFormatterBuilder();
    isoFormatter = builder.appendHourOfDay(2)
                          .appendMinuteOfHour(2)
                          .appendSecondOfMinute(2)
                          .toFormatter();
    builder.clear();
    extendedIsoFormatter = builder.appendHourOfDay(2)
                                  .appendLiteral(':')
                                  .appendMinuteOfHour(2)
                                  .appendLiteral(':')
                                  .appendSecondOfMinute(2)
                                  .toFormatter();
  }

  private final LocalTime value;
  
  /**
   * Construct using current UTC time.
   */
  public Time() {
    value = new LocalTime(DateTimeZone.UTC);
  }

  public Time(int hour, int minute, int second) {
    this(hour, minute, second, 0);
  }

  public Time(int hour, int minute, int second, int msec) {
    value = new LocalTime(hour, minute, second, msec);
  }

  protected Time(LocalTime value) {
    this.value = value;
  }

  public static Time fromIsoString(String value) {
    return new Time(isoFormatter.parseLocalTime(value));
  }

  public int hour() { return value.getHourOfDay(); }
  public int minute() { return value.getMinuteOfHour(); }
  public int second() { return value.getSecondOfMinute(); }
  public int msec() { return value.getMillisOfSecond(); }
  public int getCumulativeMsecs() { return value.getMillisOfDay(); }
  
  public String toIsoString() {
    return isoFormatter.print(value);
  }

  public String toExtendedIsoString() {
    return extendedIsoFormatter.print(value);
  }

  @Override
  public String toString() {
    return toIsoString();
  }

  @Override
  public boolean equals(Object other) {
    if (this == other)
      return true;
    
    if (!(other instanceof Time))
      return false;
    
    Time that = (Time)(other);
    return this.value.equals(that.value);
  }

  @Override
  public int hashCode() {
    return this.value.hashCode();
  }
}
