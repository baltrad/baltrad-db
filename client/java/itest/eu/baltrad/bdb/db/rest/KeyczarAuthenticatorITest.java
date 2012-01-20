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

package eu.baltrad.bdb.db.rest;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.io.*;

public class KeyczarAuthenticatorITest {
  KeyczarAuthenticator classUnderTest;

  private File getFilePath(String resource) throws Exception {
    File f = new File(this.getClass().getResource(resource).getFile());
    return f.getAbsoluteFile();
  }

  @Test
  public void constructor_path() throws Exception {
    File file = getFilePath("fixtures/testkey");
    classUnderTest = new KeyczarAuthenticator(file);
  }

  @Test
  public void constructor_invalidPath() throws Exception {
    File file = new File("/path/to/nxkey");
    try {
      classUnderTest = new KeyczarAuthenticator(file);
      fail("expected RuntimeException");
    } catch (RuntimeException e) { }
  }

}
