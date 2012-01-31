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

import static org.easymock.EasyMock.*;
import org.easymock.EasyMockSupport;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.io.*;
import java.util.*;

public class FileCacheTest extends EasyMockSupport {
  private static interface FileCacheMethods {
    public void deleteFile(File path);
  }
  
  FileCacheMethods methods;
  FileCache classUnderTest;

  @Before
  public void setup() {
    methods = createMock(FileCacheMethods.class);
    classUnderTest = new FileCache(3) {
      @Override
      protected void deleteFile(File path) {
        methods.deleteFile(path);
      }
    };
  }

  @Test
  public void put_overflow() {
    UUID uuid1 = UUID.randomUUID();
    UUID uuid2 = UUID.randomUUID();
    UUID uuid3 = UUID.randomUUID();
    UUID uuid4 = UUID.randomUUID();
    UUID uuid5 = UUID.randomUUID();

    methods.deleteFile(new File(uuid1.toString()));
    methods.deleteFile(new File(uuid2.toString()));
    replayAll();

    classUnderTest.put(uuid1, new File(uuid1.toString()));
    classUnderTest.put(uuid2, new File(uuid2.toString()));
    classUnderTest.put(uuid3, new File(uuid3.toString()));
    classUnderTest.put(uuid4, new File(uuid4.toString()));
    classUnderTest.put(uuid5, new File(uuid5.toString()));

    verifyAll();
  }

  @Test
  public void put_getChangesOrder() {
    UUID uuid1 = UUID.randomUUID();
    UUID uuid2 = UUID.randomUUID();
    UUID uuid3 = UUID.randomUUID();
    UUID uuid4 = UUID.randomUUID();
    UUID uuid5 = UUID.randomUUID();

    methods.deleteFile(new File(uuid2.toString()));
    methods.deleteFile(new File(uuid3.toString()));
    replayAll();
  
    classUnderTest.put(uuid1, new File(uuid1.toString()));
    classUnderTest.put(uuid2, new File(uuid2.toString()));
    classUnderTest.put(uuid3, new File(uuid3.toString()));
    classUnderTest.get(uuid1);
    classUnderTest.put(uuid4, new File(uuid4.toString()));
    classUnderTest.put(uuid5, new File(uuid5.toString()));
    verifyAll();    
  }

  @Test
  public void put_putChangesOrder() {
    UUID uuid1 = UUID.randomUUID();
    UUID uuid2 = UUID.randomUUID();
    UUID uuid3 = UUID.randomUUID();
    UUID uuid4 = UUID.randomUUID();
    UUID uuid5 = UUID.randomUUID();

    methods.deleteFile(new File(uuid2.toString()));
    methods.deleteFile(new File(uuid3.toString()));
    replayAll();
  
    classUnderTest.put(uuid1, new File(uuid1.toString()));
    classUnderTest.put(uuid2, new File(uuid2.toString()));
    classUnderTest.put(uuid3, new File(uuid3.toString()));
    classUnderTest.put(uuid1, new File(uuid1.toString()));
    classUnderTest.put(uuid4, new File(uuid4.toString()));
    classUnderTest.put(uuid5, new File(uuid5.toString()));
    verifyAll();    
  }

  @Test
  public void remove() {
    UUID uuid = UUID.randomUUID();
    
    methods.deleteFile(new File(uuid.toString()));
    replayAll();
    
    classUnderTest.put(uuid, new File(uuid.toString()));
    classUnderTest.remove(uuid);
    verifyAll();
  }

  @Test
  public void remove_nx() {
    replayAll();

    classUnderTest.remove(UUID.randomUUID());
    verifyAll();
  }
}
