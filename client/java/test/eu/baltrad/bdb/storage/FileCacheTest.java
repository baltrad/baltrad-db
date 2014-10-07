/*
Copyright (C) 2009-2012 Swedish Meteorological and Hydrological Institute, SMHI,

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

import static org.easymock.EasyMock.*;

import java.io.File;
import java.util.UUID;

import org.easymock.EasyMockSupport;
import org.junit.After;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;

public class FileCacheTest extends EasyMockSupport  {
  private LinkedFileCache linkedCache = null;
  private FileCache classUnderTest = null;
  
  @Before
  public void setup() {
    linkedCache = createMock(LinkedFileCache.class);
    classUnderTest = new FileCache(linkedCache);
  }
  
  @After
  public void tearDown() {
    linkedCache = null;
    classUnderTest = null;
  }
  
  @Test
  public void testPut() throws Exception {
    UUID uuid1 = UUID.randomUUID();
    File f = new File(uuid1.toString());
    
    expect(linkedCache.put(uuid1, f)).andReturn(null);
    
    replayAll();
    
    classUnderTest.put(uuid1, f);
    
    verifyAll();
  }
  
  @Test
  public void testGet() throws Exception {
    UUID uuid1 = UUID.randomUUID();
    File f = new File(uuid1.toString());
    
    expect(linkedCache.get(uuid1)).andReturn(f);
    
    replayAll();
    
    File result = classUnderTest.get(uuid1);
    
    verifyAll();
    Assert.assertTrue(f == result);
  }
  
  @Test
  public void testRemove() throws Exception {
    UUID uuid1 = UUID.randomUUID();
    File f = new File(uuid1.toString());

    expect(linkedCache.remove(uuid1)).andReturn(f);

    replayAll();

    classUnderTest.remove(uuid1);
    
    verifyAll();
  }

  class FCThread extends Thread {
    private FileCache fc = null;
    private boolean doRemove = false;
    public FCThread(FileCache fc) {
      this.fc = fc;
    }
    public FCThread(FileCache fc, boolean remove) {
      this.fc = fc;
      this.doRemove = remove;
    }
    public void run() {
      for (int i = 0; i < 100; i++) {
        UUID uuid = UUID.randomUUID();
        fc.put(uuid, new File(uuid.toString()));
        if (doRemove) {
          fc.remove(uuid);
        }
      }
    }
  }

  class CountingLinkedFileCache extends LinkedFileCache {
    private static final long serialVersionUID = 1L;
    int nrDeletes = 0;
    CountingLinkedFileCache(int fileLimit){
      super(fileLimit);
    }
    @Override
    protected void deleteFile(File path) {
      nrDeletes++;
    }
  };
  
  @Test
  public void testRunit() throws Exception {
    CountingLinkedFileCache flfc = new CountingLinkedFileCache(20);
    final FileCache fc = new FileCache(flfc);
    Thread[] threads = new Thread[]{new FCThread(fc), new FCThread(fc), new FCThread(fc), new FCThread(fc, true), new FCThread(fc, true)};
    for (Thread t : threads) {
      t.start();
    }
    for (Thread t : threads) {
      t.join();
    }
    Assert.assertEquals(flfc.size(), threads.length * 100 - flfc.nrDeletes);
    //System.out.println("Nr updates = " + threads.length*100 + ", nrDeletes = " + flfc.nrDeletes + ", nrItems = " + flfc.size());
  }
}
