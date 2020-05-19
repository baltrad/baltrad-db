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

public class TimeTest {
  private Time classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new Time(12, 13, 14, 15);
  }

  @Test
  public void toIsoString() {
    assertEquals("121314", classUnderTest.toIsoString());
  }

  @Test
  public void toExtendedIsoString() {
    assertEquals("12:13:14", classUnderTest.toExtendedIsoString());
  }

  @Test
  public void fromIsoString() {
    classUnderTest = Time.fromIsoString("131415");
    assertEquals(classUnderTest, new Time(13, 14, 15));
  }

  @Test
  public void fromIsoString_invalid() {
    try {
      classUnderTest = Time.fromIsoString("");
      fail("expected IllegalArgumentException");
    } catch (IllegalArgumentException e) { }
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
    assertTrue(classUnderTest.equals(new Time(12, 13, 14, 15)));
  }

  @Test
  public void equals_differentValue() {
    assertFalse(classUnderTest.equals(new Time(12, 13, 14, 16)));
  }

  @Test
  public void hashCode_sameValue() {
    Time other = new Time(12, 13, 14, 15);
    assertEquals(classUnderTest.hashCode(), other.hashCode());
  }
}
