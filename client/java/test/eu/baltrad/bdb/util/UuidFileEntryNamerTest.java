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

package eu.baltrad.bdb.util;

import eu.baltrad.bdb.db.FileEntry;

import static org.easymock.EasyMock.*;
import org.easymock.EasyMockSupport;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.util.UUID;

public class UuidFileEntryNamerTest extends EasyMockSupport {
  private UuidFileEntryNamer classUnderTest;
  
  @Before
  public void setup() {
    classUnderTest = new UuidFileEntryNamer();
  }

  @Test
  public void name() {
    UUID uuid = UUID.fromString("abcdef00-0000-0000-0004-000000000001");
    FileEntry fileEntry = createMock(FileEntry.class);

    expect(fileEntry.getUuid())
      .andReturn(uuid);
    replayAll();
    
    String result = classUnderTest.name(fileEntry);
    assertEquals("abcdef00-0000-0000-0004-000000000001.h5", result);
    verifyAll();
  }
}
