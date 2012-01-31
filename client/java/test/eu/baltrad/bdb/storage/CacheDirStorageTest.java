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

public class CacheDirStorageTest extends EasyMockSupport {
  private static interface CacheDirStorageMethods {
    public void copyInputStreamToFile(InputStream src, File dst);
    public File store(FileEntry entry, InputStream fileContent);
  }
  
  private FileCache cache;
  private CacheDirStorageMethods methods;
  private CacheDirStorage classUnderTest;

  @Before
  public void setup() {
    cache = createMock(FileCache.class);
    methods = createMock(CacheDirStorageMethods.class);
    classUnderTest = new CacheDirStorage(new File("/path"), cache) {
      @Override
      protected void copyInputStreamToFile(InputStream src, File dst) {
        methods.copyInputStreamToFile(src, dst);
      }
    };
  }

  @Test
  public void store() {
    InputStream stream = createMock(InputStream.class);
    UUID uuid = UUID.fromString("abcdef00-0000-0000-0004-000000000001");
    File dst = new File("/path/abcdef00-0000-0000-0004-000000000001");
    FileEntry entry = createMock(FileEntry.class);

    expect(entry.getUuid())
      .andReturn(uuid).atLeastOnce();
    expect(cache.get(uuid))
      .andReturn(null);
    methods.copyInputStreamToFile(stream, dst);
    expect(cache.put(uuid, dst))
      .andReturn(null);
    replayAll();

    assertEquals(dst, classUnderTest.store(entry, stream));
    verifyAll();
  }

  @Test
  public void store_existing() {
    InputStream stream = createMock(InputStream.class);
    UUID uuid = UUID.fromString("abcdef00-0000-0000-0004-000000000001");
    File dst = new File("/path/abcdef00-0000-0000-0004-000000000001");
    FileEntry entry = createMock(FileEntry.class);

    expect(entry.getUuid())
      .andReturn(uuid);
    expect(cache.get(uuid))
      .andReturn(dst);
    replayAll();

    assertEquals(dst, classUnderTest.store(entry, stream));
    verifyAll();    
  }

  @Test
  public void store_FileEntry() throws Exception {
    classUnderTest = new CacheDirStorage(new File("/path"), 2) {
      @Override
      public File store(FileEntry entry, InputStream content) {
        return methods.store(entry, content);
      }
    };

    InputStream stream = createMock(InputStream.class);
    File dst = new File("/path/abcdef00-0000-0000-0004-000000000001");
    FileEntry entry = createMock(FileEntry.class);

    expect(entry.getContentStream())
      .andReturn(stream);
    expect(methods.store(entry, stream))
      .andReturn(dst);
    stream.close();
    replayAll();

    assertEquals(dst, classUnderTest.store(entry));
    verifyAll();
  }

  @Test
  public void remove() {
    UUID uuid = UUID.fromString("abcdef00-0000-0000-0004-000000000001");

    FileEntry entry = createMock(FileEntry.class);

    expect(entry.getUuid())
      .andReturn(uuid);
    expect(cache.remove(uuid))
      .andReturn(null);
    replayAll();
    
    classUnderTest.remove(entry);
    verifyAll();
  }

}
