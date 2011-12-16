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

import eu.baltrad.bdb.util.Date;
import eu.baltrad.bdb.util.Time;

public class Attribute extends BaseNode implements AttributeValue {
  private AttributeValue value;

  public Attribute(String name) {
    this(name, "");
  }

  public Attribute(String name, double value) {
    super(name);
    setValue(value);
  }

  public Attribute(String name, long value) {
    super(name);
    setValue(value);
  }

  public Attribute(String name, String value) {
    super(name);
    setValue(value);
  }

  public void setValue(double value) {
    this.value = new AttributeValueDouble(value);
  }

  public void setValue(long value) {
    this.value = new AttributeValueLong(value);
  }
  
  public void setValue(String value) {
    this.value = new AttributeValueString(value);
  }

  @Override
  public Type getType() { return value.getType(); }

  @Override
  public boolean isDouble() { return value.isDouble(); }

  @Override
  public boolean isLong() { return value.isLong(); }

  @Override
  public boolean isString() { return value.isString(); }

  @Override
  public Boolean toBoolean() { return value.toBoolean(); }

  @Override
  public Double toDouble() { return value.toDouble(); }

  @Override
  public Long toLong() { return value.toLong(); }

  @Override
  public String toString() { return value.toString(); }

  @Override
  public Date toDate() { return value.toDate(); }

  @Override
  public Time toTime() { return value.toTime(); }

}
