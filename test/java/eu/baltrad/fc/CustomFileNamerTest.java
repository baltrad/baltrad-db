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

import eu.baltrad.fc.FileNamer;

import junit.framework.TestCase;

public class CustomFileNamerTest extends TestCase {
  public void testCustomFileNamer() {
    FileNamer namer = new FileNamer() {
      protected String do_name(Oh5File file) {
        return "file";
      }
    };

    Oh5File file = new Oh5PhysicalFile();
    assertEquals("file", namer.name(file));
  }
}
