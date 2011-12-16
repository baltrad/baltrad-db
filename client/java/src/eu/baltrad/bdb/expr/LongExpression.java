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

package eu.baltrad.bdb.expr;

public final class LongExpression extends BaseExpression {
  private final Long value;

  public LongExpression(long value) {
    this.value = value;
  }

  public LongExpression(LongExpression other) {
    this(other.value);
  }

  @Override
  public Type getType() {
    return Type.LONG;
  }
  
  @Override
  public Long toLong() {
    return value;
  }

  @Override
  public Double toDouble() {
    return value.doubleValue();
  }

  @Override
  public String toString() {
    return value.toString();
  }

  @Override
  public boolean equals(Object other) {
    if (this == other)
      return true;
    
    if (!(other instanceof LongExpression))
      return false;
    
    LongExpression that = (LongExpression)(other);
    return this.value.equals(that.value);


  }

  @Override
  public int hashCode() {
    return this.value.hashCode();
  }
}
