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
along with baltrad-db.  If not, see <http://www.gnu.org/licenses/>.
*/

package eu.baltrad.fc;

import eu.baltrad.fc.test.TestSwig;

import junit.framework.TestCase;

public class TestQString extends TestCase {
  private TestSwig swig;
  private String str;

  public void setUp() {
    swig = new TestSwig();
    str = new String("\u00F6\u00E4\u00FC\u20AC");
  }

  public void test_qstring_to_string() {
    String qstr = swig.qstring_to_string(str);
    assertEquals(qstr, str);
  }

  public void test_qstringconstref_to_string() {
    String qstr = swig.qstringconstref_to_string(str);
    assertEquals(qstr, str);
  }

  public void test_string_to_qstring() {
    String qstr = swig.string_to_qstring(str);
    assertEquals(qstr, str);
  }

  public void test_string_to_qstringconstref() {
    String qstr = swig.string_to_qstringconstref(str);
    assertEquals(qstr, str);
  }

}
