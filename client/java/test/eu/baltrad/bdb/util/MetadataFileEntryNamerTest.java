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
import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.MetadataNamer;

import static org.easymock.EasyMock.*;
import org.easymock.EasyMockSupport;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class MetadataFileEntryNamerTest extends EasyMockSupport {
  private MetadataNamer metadataNamer;
  private MetadataFileEntryNamer classUnderTest;
  
  @Before
  public void setup() {
    metadataNamer = createMock(MetadataNamer.class);
    classUnderTest = new MetadataFileEntryNamer(metadataNamer);
  }

  @Test
  public void name() {
    FileEntry fileEntry = createMock(FileEntry.class);
    Metadata metadata = createMock(Metadata.class);

    expect(fileEntry.getMetadata())
      .andReturn(metadata);
    expect(metadataNamer.name(metadata))
      .andReturn("metadata_name");
    replayAll();
    
    assertEquals("metadata_name", classUnderTest.name(fileEntry));
    verifyAll();
  }
}
