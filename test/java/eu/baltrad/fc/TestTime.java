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

public class TestTime extends TestCase {

  public void test_to_string() {
    Time t = new Time(12, 00, 01);
    assertEquals(t.to_string("hhmmss"), "120001");
  }

  public void test_equals() {
    Time t1 = new Time(0, 0, 1);
    Time t2 = new Time(1, 1, 1);
    Time t3 = new Time(0, 0, 1);

    assertTrue(t1.equals(t1));
    assertFalse(t1.equals(t2));
    assertTrue(t1.equals(t3));
  }

  public void test_compareTo() {
    Time t1 = new Time(12, 0);
    Time t2 = new Time(12, 0, 0, 1);

    assertEquals(t1.compareTo(t1), 0);
    assertEquals(t2.compareTo(t2), 0);
    assertEquals(t1.compareTo(t2), -1);
    assertEquals(t2.compareTo(t1), 1);
  }

  public void test_implementsComparable() {
    Time t1 = new Time(12, 0);
    assertTrue(t1 instanceof Comparable);
  }

  public void test_add() {
    Time t = new Time(0, 0, 1);
    Time nt = t.add(new TimeDelta().add_seconds(60));

    assertEquals(new Time(0, 1, 1), nt);
    assertFalse(t.equals(nt));
  }
}
