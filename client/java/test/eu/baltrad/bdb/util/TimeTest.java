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
}
