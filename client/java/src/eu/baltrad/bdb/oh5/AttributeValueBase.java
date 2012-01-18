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

public abstract class AttributeValueBase implements AttributeValue {
  @Override
  public boolean isDouble() {
    return getType() == Type.DOUBLE;
  }

  @Override
  public boolean isNull() {
    return false;
  }
  
  @Override
  public boolean isLong() {
    return getType() == Type.LONG;
  }
  
  @Override
  public boolean isString() {
    return getType() == Type.STRING;
  }

  @Override
  public Boolean toBoolean() {
    return null;
  }
  
  @Override
  public Double toDouble() {
    return null;
  }
  
  @Override
  public Long toLong() {
    return null;
  }

  @Override
  public Date toDate() {
    return null;
  }

  @Override
  public Time toTime() {
    return null;
  }
}
