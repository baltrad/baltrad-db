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

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class DateTimeTest {
  private DateTime classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new DateTime(2011, 12, 13, 14, 15);
  }

  @Test
  public void equals_sameInstance() {
    assertTrue(classUnderTest.equals(classUnderTest));
  }

  @Test
  public void equals_otherType() {
    assertFalse(classUnderTest.equals(Double.valueOf(1)));
  }

  @Test
  public void equals_sameValue() {
    assertTrue(classUnderTest.equals(new DateTime(2011, 12, 13, 14, 15)));
  }

  @Test
  public void equals_differentValue() {
    assertFalse(classUnderTest.equals(new DateTime(2011, 12, 13, 14, 15, 16)));
  }

  @Test
  public void hashCode_sameValue() {
    DateTime other = new DateTime(2011, 12, 13, 14, 15);
    assertEquals(classUnderTest.hashCode(), other.hashCode());
  }
}
