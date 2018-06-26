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

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.util.*;

public class SourceTest {
  private Source classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new Source();
  }

  @Test
  public void get_missing() {
    assertNull(classUnderTest.get("foo"));
  }

  @Test
  public void get() {
    classUnderTest.put("key", "value");
    assertEquals("value", classUnderTest.get("key"));
  }

  @Test
  public void getKeys() {
    classUnderTest.put("key1", "value");
    classUnderTest.put("key2", "value");

    Collection<String> keys = classUnderTest.getKeys();
    assertEquals(2, keys.size());
    assertTrue(keys.contains("key1"));
    assertTrue(keys.contains("key2"));
  }

  @Test
  public void fromString() {
    classUnderTest = Source.fromString("WMO:12345,NOD:eefoo");
    assertEquals(2, classUnderTest.getKeys().size());
    assertEquals("12345", classUnderTest.get("WMO"));
    assertEquals("eefoo", classUnderTest.get("NOD"));
  }
  
  @Test
  public void fromString_incompleteKeyValuePair() {
    classUnderTest = Source.fromString("WMO:12345,NOD:eefoo,CMT:");
    assertEquals(2, classUnderTest.getKeys().size());
    assertEquals("12345", classUnderTest.get("WMO"));
    assertEquals("eefoo", classUnderTest.get("NOD"));
    assertEquals(null, classUnderTest.get("CMT"));
  }

  @Test
  public void fromString_empty() {
    try {
      classUnderTest = Source.fromString("");
      fail("expected InvalidSource");
    } catch (InvalidSource e) { }
  }

  @Test
  public void fromString_invalid1() {
    try {
      classUnderTest = Source.fromString("key:value,key");
      fail("expected InvalidSource");
    } catch (InvalidSource e) { }
  }

  @Test
  public void fromString_invalid2() {
    try {
      classUnderTest = Source.fromString("key:key:value,key");
      fail("expected InvalidSource");
    } catch (InvalidSource e) { }
  }

}
