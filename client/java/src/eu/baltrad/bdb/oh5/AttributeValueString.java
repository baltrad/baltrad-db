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

public final class AttributeValueString extends AttributeValueBase {
  private final String value;

  public AttributeValueString(String value) {
    this.value = value;
  }

  @Override
  public Type getType() {
    return Type.STRING;
  }

  @Override
  public Boolean toBoolean() {
    if ("True".equals(value)) {
      return true;
    } else if ("False".equals(value)) {
      return false;
    } else {
      return null;
    }
  }

  @Override
  public String toString() {
    return value.toString();
  }

  @Override
  public Date toDate() {
    return Date.fromIsoString(value);
  }

  @Override
  public Time toTime() {
    return Time.fromIsoString(value);
  }
}
