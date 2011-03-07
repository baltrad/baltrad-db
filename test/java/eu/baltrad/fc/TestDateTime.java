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

package eu.baltrad.fc;

import junit.framework.TestCase;

public class TestDateTime extends TestCase {

  public void test_to_string() {
    DateTime dt = new DateTime(2000, 5, 6, 12, 0, 1);
    assertEquals(dt.to_string("hhmmss yyyyMMdd"), "120001 20000506");
  }

  public void test_equals() {
    DateTime dt1 = new DateTime(2000, 5, 6, 0, 0, 1);
    DateTime dt2 = new DateTime(2000, 5, 6, 1, 1, 1);
    DateTime dt3 = new DateTime(2000, 5, 6, 0, 0, 1);

    assertTrue(dt1.equals(dt1));
    assertFalse(dt1.equals(dt2));
    assertTrue(dt1.equals(dt3));
  }

  public void test_compareTo() {
    DateTime dt1 = new DateTime(2000, 5, 6);
    DateTime dt2 = new DateTime(2000, 5, 6, 0, 0, 1);
    
    assertEquals(dt1.compareTo(dt1), 0);
    assertEquals(dt2.compareTo(dt2), 0);
    assertEquals(dt1.compareTo(dt2), -1);
    assertEquals(dt2.compareTo(dt1), 1);
  }

  public void test_implementsComparable() {
    DateTime dt1 = new DateTime(2000, 5, 6);
    
    assertTrue(dt1 instanceof Comparable);
  }

  public void test_add() {
    DateTime dt = new DateTime(2000, 5, 6, 0, 0, 1);
    DateTime ndt = dt.add(new TimeDelta().add_days(1).add_seconds(60));

    assertEquals(new DateTime(2000, 5, 7, 0, 1, 1), ndt);
    assertFalse(dt.equals(ndt));
  }
}
