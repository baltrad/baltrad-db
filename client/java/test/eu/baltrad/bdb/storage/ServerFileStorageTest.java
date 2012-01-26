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

package eu.baltrad.bdb.storage;

import eu.baltrad.bdb.db.FileEntry;

import static org.easymock.EasyMock.*;
import org.easymock.EasyMockSupport;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.io.*;
import java.util.UUID;

public class ServerFileStorageTest extends EasyMockSupport {
  private ServerFileStorage classUnderTest;
  
  @Before
  public void setup() {
    classUnderTest = new ServerFileStorage(new File("/path"), 3);
  }

  @Test
  public void store_FileEntry_InputStream() {
    UUID uuid = UUID.fromString("abcdef00-0000-0000-0004-000000000001");
    FileEntry entry = createMock(FileEntry.class);
    InputStream stream = createMock(InputStream.class);

    expect(entry.getUuid())
      .andReturn(uuid);
    replayAll();

    File result = classUnderTest.store(entry, stream);
    File expected =
      new File("/path/a/b/c/abcdef00-0000-0000-0004-000000000001");
    assertEquals(expected, result);
    verifyAll();
  }

  @Test
  public void store_FileEntry() {
    UUID uuid = UUID.fromString("abcdef00-0000-0000-0004-000000000001");

    FileEntry entry = createMock(FileEntry.class);
    expect(entry.getUuid())
      .andReturn(uuid);
    replayAll();

    File result = classUnderTest.store(entry);
    File expected =
      new File("/path/a/b/c/abcdef00-0000-0000-0004-000000000001");
    assertEquals(expected, result);
    verifyAll();
  }
}
