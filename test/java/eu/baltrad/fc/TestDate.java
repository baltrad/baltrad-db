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
    Date d = new Date(2000, 05, 01);
    assertEquals(d.to_string("yyyyMMdd"), "20000501");
  }

  public void test_add_days_positive() {
    Date d = new Date(2000, 1, 1);
    Date nd = d.add_days(1);
    assertEquals(nd.year(), 2000);
    assertEquals(nd.month(), 1);
    assertEquals(nd.day(), 2);
  }

  public void test_add_days_negative() {
    Date d = new Date(2000, 1, 1);
    Date nd = d.add_days(-1);
    assertEquals(nd.year(), 1999);
    assertEquals(nd.month(), 12);
    assertEquals(nd.day(), 31);
  }

  public void test_add_months_positive() {
    Date d = new Date(2000, 1, 1);
    Date nd = d.add_months(1);
    assertEquals(nd.year(), 2000);
    assertEquals(nd.month(), 2);
    assertEquals(nd.day(), 1);
  }
  
  public void test_add_months_negative() {
    Date d = new Date(2000, 1, 1);
    Date nd = d.add_months(-1);
    assertEquals(nd.year(), 1999);
    assertEquals(nd.month(), 12);
    assertEquals(nd.day(), 1);
  }

  public void test_add_years_positive() {
    Date d = new Date(2000, 1, 1);
    Date nd = d.add_years(1);
    assertEquals(nd.year(), 2001);
    assertEquals(nd.month(), 1);
    assertEquals(nd.day(), 1);
  }

  public void test_add_years_negative() {
    Date d = new Date(2000, 1, 1);
    Date nd = d.add_years(-1);
    assertEquals(nd.year(), 1999);
    assertEquals(nd.month(), 1);
    assertEquals(nd.day(), 1);
  }

}
