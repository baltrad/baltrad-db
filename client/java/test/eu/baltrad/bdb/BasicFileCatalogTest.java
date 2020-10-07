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

package eu.baltrad.bdb;

import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.db.Database;
import eu.baltrad.bdb.storage.LocalStorage;

import org.apache.commons.io.input.NullInputStream;

import static org.easymock.EasyMock.*;
import org.easymock.EasyMockSupport;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.io.*;
import java.util.UUID;

public class BasicFileCatalogTest extends EasyMockSupport {
  private Database database;
  private LocalStorage localStorage;
  private BasicFileCatalog classUnderTest;
  
  @Before
  public void setup() {
    database = createMock(Database.class);
    localStorage = createMock(LocalStorage.class);
    classUnderTest = new BasicFileCatalog(database, localStorage);
    resetAll();
  }
  
  @Test
  public void store() throws Exception {
    InputStream fileContent = new NullInputStream(1024);
    FileEntry entry = createMock(FileEntry.class);
    UUID uuid = UUID.randomUUID();

    expect(database.store(isA(ByteArrayInputStream.class)))
      .andReturn(entry);
    expect(localStorage.store(same(entry), isA(ByteArrayInputStream.class)))
      .andReturn(null);
    expect(entry.getUuid()).andReturn(uuid).anyTimes();
    replayAll();

    assertSame(entry, classUnderTest.store(fileContent));

    verifyAll();
  }

  @Test
  public void store_LocalStorageFailure() throws Exception {
    InputStream fileContent = new NullInputStream(1024);
    FileEntry entry = createMock(FileEntry.class);
    UUID uuid = UUID.randomUUID();

    expect(database.store(isA(ByteArrayInputStream.class)))
      .andReturn(entry);
    expect(localStorage.store(same(entry), isA(ByteArrayInputStream.class)))
      .andThrow(new RuntimeException());
    expect(entry.getUuid()).andReturn(uuid).anyTimes();
    replayAll();

    assertSame(entry, classUnderTest.store(fileContent));
    verifyAll();
  }

  @Test
  public void remove() {
    UUID uuid = UUID.randomUUID();
    FileEntry entry = createMock(FileEntry.class);
    
    expect(entry.getUuid())
      .andReturn(uuid).anyTimes();
    expect(database.removeFileEntry(uuid))
      .andReturn(true);
    localStorage.remove(entry);
    replayAll();

    classUnderTest.remove(entry);
    verifyAll();
  }

  @Test
  public void remove_LocalStorageFailure() {
    UUID uuid = UUID.randomUUID();
    FileEntry entry = createMock(FileEntry.class);

    expect(entry.getUuid())
      .andReturn(uuid).anyTimes();
    expect(database.removeFileEntry(uuid))
      .andReturn(true);
    localStorage.remove(entry);
    expectLastCall().andThrow(new RuntimeException());
    replayAll();

    classUnderTest.remove(entry);
    verifyAll();
  }

  @Test
  public void getLocalPathForUuid() {
    UUID uuid = UUID.randomUUID();
    FileEntry entry = createMock(FileEntry.class);
    File file = new File("");

    expect(database.getFileEntry(uuid)).andReturn(entry);
    expect(localStorage.store(entry)).andReturn(file);
    replayAll();

    assertSame(file, classUnderTest.getLocalPathForUuid(uuid));
    verifyAll();
  }

}
