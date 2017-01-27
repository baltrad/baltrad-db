/*
Copyright 2017 Swedish Meteorological and Hydrological Institute, SMHI,

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

import static org.easymock.EasyMock.expect;
import static org.junit.Assert.assertEquals;

import java.util.UUID;

import org.easymock.EasyMockSupport;
import org.junit.Before;
import org.junit.Test;

import eu.baltrad.bdb.db.Database;
import eu.baltrad.bdb.db.FileEntry;
import eu.baltrad.bdb.oh5.Attribute;
import eu.baltrad.bdb.oh5.Metadata;

public class RestfulFileEntryCacheTest extends EasyMockSupport {

  RestfulFileEntryCache classUnderTest;
  
  Database databaseMock;

  @Before
  public void setUp() {
    classUnderTest = new RestfulFileEntryCache(10);
    
    databaseMock = createMock(Database.class);
  }
  
  private RestfulFileEntry setupTestFileEntry(UUID uuid) {
    Metadata metadata = createMock(Metadata.class);
    RestfulFileEntry entry = new RestfulFileEntry(databaseMock, metadata);
    
    expect(metadata.getAttribute("/_bdb/uuid")).andReturn(new Attribute("/_bdb/uuid", uuid.toString()));
    
    return entry;
  }

  @Test
  public void getFileEntry_emptyCache() {
    UUID uuid = new UUID(589, 877);
    
    FileEntry result = classUnderTest.getFileEntry(uuid);
    
    assertEquals(null, result);
    
    assertEquals(classUnderTest.getCurrentCacheHitRate(), 0.0, 0.001);
  }
  
  @Test
  public void getFileEntry_oneEntry() {
    UUID uuid1 = new UUID(589, 877);
    UUID uuid2 = new UUID(88, 9156);
    
    RestfulFileEntry entry1 = setupTestFileEntry(uuid1);
    
    replayAll();
    
    classUnderTest.addFileEntry(entry1);
    FileEntry result = classUnderTest.getFileEntry(uuid2);
    assertEquals(null, result);
    result = classUnderTest.getFileEntry(uuid1);
    assertEquals(entry1, result);
    
    assertEquals(classUnderTest.getCurrentCacheHitRate(), 0.50, 0.001);
    
    verifyAll();
  }
  
  @Test
  public void getFileEntry_removeEntry() {
    int cacheSize = 2;
    classUnderTest = new RestfulFileEntryCache(cacheSize);
    
    UUID uuid1 = new UUID(589, 877);
    UUID uuid2 = new UUID(88, 9156);
    
    RestfulFileEntry entry1 = setupTestFileEntry(uuid1);
    RestfulFileEntry entry2 = setupTestFileEntry(uuid2);
    
    replayAll();
    
    classUnderTest.addFileEntry(entry1);
    classUnderTest.addFileEntry(entry2);
    
    assertEquals(classUnderTest.getNoOfEntriesInCache(), 2);
    
    FileEntry result = classUnderTest.getFileEntry(uuid1);
    assertEquals(entry1, result);
    result = classUnderTest.getFileEntry(uuid2);
    assertEquals(entry2, result);
    
    classUnderTest.removeFileEntry(uuid1);
    assertEquals(classUnderTest.getNoOfEntriesInCache(), 1);
    result = classUnderTest.getFileEntry(uuid1);
    assertEquals(null, result);
    result = classUnderTest.getFileEntry(uuid2);
    assertEquals(entry2, result);
    
    classUnderTest.removeFileEntry(uuid2);
    assertEquals(classUnderTest.getNoOfEntriesInCache(), 0);
    result = classUnderTest.getFileEntry(uuid1);
    assertEquals(null, result);
    result = classUnderTest.getFileEntry(uuid2);
    assertEquals(null, result);
    
    assertEquals(classUnderTest.getCurrentCacheHitRate(), 0.50, 0.001);
    
    verifyAll();
  }
  
  @Test
  public void getFileEntry_fullCache() {
    int cacheSize = 2;
    classUnderTest = new RestfulFileEntryCache(cacheSize);
    
    UUID uuid1 = new UUID(589, 877);
    UUID uuid2 = new UUID(88, 9156);
    UUID uuid3 = new UUID(4866, 32);
    
    RestfulFileEntry entry1 = setupTestFileEntry(uuid1);
    RestfulFileEntry entry2 = setupTestFileEntry(uuid2);
    RestfulFileEntry entry3 = setupTestFileEntry(uuid3);
    
    replayAll();
    
    classUnderTest.addFileEntry(entry1);
    classUnderTest.addFileEntry(entry2);
    classUnderTest.addFileEntry(entry3);
    
    FileEntry result = classUnderTest.getFileEntry(uuid1);
    // since uuid1 was the oldest (added first), we expect that it has been cleared from cache
    assertEquals(null, result);
    result = classUnderTest.getFileEntry(uuid2);
    assertEquals(entry2, result);
    result = classUnderTest.getFileEntry(uuid3);
    assertEquals(entry3, result);
    
    assertEquals(classUnderTest.getCurrentCacheHitRate(), 0.667, 0.0005);
    
    verifyAll();
  }
  
  @Test
  public void getFileEntry_eldestFileRemoved() {
    int cacheSize = 3;
    classUnderTest = new RestfulFileEntryCache(cacheSize);
    
    UUID uuid1 = new UUID(589, 877);
    UUID uuid2 = new UUID(88, 9156);
    UUID uuid3 = new UUID(4866, 32);
    UUID uuid4 = new UUID(8773, 6448);
    
    RestfulFileEntry entry1 = setupTestFileEntry(uuid1);
    RestfulFileEntry entry2 = setupTestFileEntry(uuid2);
    RestfulFileEntry entry3 = setupTestFileEntry(uuid3);
    RestfulFileEntry entry4 = setupTestFileEntry(uuid4);
    
    replayAll();
    
    classUnderTest.addFileEntry(entry1);
    classUnderTest.addFileEntry(entry2);
    classUnderTest.addFileEntry(entry3);
    // "touch" entry1, to make it youngest
    classUnderTest.getFileEntry(uuid1);
    
    // add one more entry to the full cach to push out the oldest file, which should be entry 2
    classUnderTest.addFileEntry(entry4);
    
    FileEntry result = classUnderTest.getFileEntry(uuid1);
    assertEquals(entry1, result);
    result = classUnderTest.getFileEntry(uuid2);
    // uuid2 is expected to be seen as oldest. It should therefore have been cleared from cache
    assertEquals(null, result);
    result = classUnderTest.getFileEntry(uuid3);
    assertEquals(entry3, result);
    result = classUnderTest.getFileEntry(uuid4);
    assertEquals(entry4, result);
    
    assertEquals(classUnderTest.getCurrentCacheHitRate(), 0.8, 0.0005);
    
    verifyAll();
  }

}
