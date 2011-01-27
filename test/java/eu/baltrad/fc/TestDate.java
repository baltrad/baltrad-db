/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

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

package eu.baltrad.fc;

import eu.baltrad.fc.Date;

import junit.framework.TestCase;

public class TestDate extends TestCase {

  public void test_to_string() {
    Date d = new Date(2000, 5, 1);
    assertEquals(d.to_string("yyyyMMdd"), "20000501");
  }

  public void test_equals() {
    Date d1 = new Date(2000, 5, 1);
    Date d2 = new Date(2000, 6, 1);
    Date d3 = new Date(2000, 5, 1);

    assertTrue(d1.equals(d1));
    assertFalse(d1.equals(d2));
    assertTrue(d1.equals(d3));
  }

  public void test_compareTo() {
    Date d1 = new Date(2000, 2, 1);
    Date d2 = new Date(2000, 2, 2);

    assertEquals(d1.compareTo(d1), 0);
    assertEquals(d2.compareTo(d2), 0);
    assertEquals(d1.compareTo(d2), -1);
    assertEquals(d2.compareTo(d1), 1);
  }

  public void test_add() {
    Date d = new Date(2000, 1, 1);
    Date nd = d.add(new TimeDelta().add_days(1));

    assertEquals(new Date(2000, 1, 2), nd);
  }

}
