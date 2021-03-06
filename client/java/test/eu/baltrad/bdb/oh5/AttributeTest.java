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

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class AttributeTest {
  Attribute classUnderTest;

  @Test
  public void constructor() {
    classUnderTest = new Attribute("foo");
    assertEquals("foo", classUnderTest.getName());
    assertTrue(classUnderTest.isNull());
    assertEquals(AttributeValue.Type.NULL, classUnderTest.getType());
  }

  @Test
  public void toDate() {
    classUnderTest = new Attribute("foo", "20111213");
    assertEquals(new Date(2011, 12, 13), classUnderTest.toDate());
  }

  @Test
  public void toTime() {
    classUnderTest = new Attribute("foo", "141516");
    assertEquals(new Time(14, 15, 16), classUnderTest.toTime());
  }
}
