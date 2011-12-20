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

import eu.baltrad.bdb.db.Database;
import eu.baltrad.bdb.oh5.Attribute;
import eu.baltrad.bdb.oh5.Group;
import eu.baltrad.bdb.oh5.Metadata;
import eu.baltrad.bdb.oh5.Source;
import eu.baltrad.bdb.util.Date;
import eu.baltrad.bdb.util.Time;

import static org.easymock.EasyMock.*;
import org.easymock.EasyMockSupport;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.io.InputStream;
import java.util.UUID;

public class RestfulFileEntryTest extends EasyMockSupport {
  Database database;
  Metadata metadata;
  RestfulFileEntry classUnderTest;

  @Before
  public void setUp() {
    database = createMock(Database.class);
    metadata = new Metadata();
    classUnderTest = new RestfulFileEntry(database, metadata);
  }

  @Test
  public void getUuid() {
    String uuidStr = "00000000-0000-0000-0004-000000000001";
    metadata.addNode("/", new Group("_bdb"));
    metadata.addNode("/_bdb", new Attribute("uuid", uuidStr));

    assertEquals(UUID.fromString(uuidStr), classUnderTest.getUuid());
  }

  @Test
  public void getSource() {
    String srcStr = "_name:foo,NOD:bar";

    metadata.addNode("/", new Group("_bdb"));
    metadata.addNode("/_bdb", new Attribute("source", srcStr));

    Source result = classUnderTest.getSource();

    assertEquals("foo", result.get("_name"));
    assertEquals("bar", result.get("NOD"));
  }

  @Test
  public void getContentSize() {
    metadata.addNode("/", new Group("_bdb"));
    metadata.addNode("/_bdb", new Attribute("file_size", 1234));

    assertEquals(1234, classUnderTest.getContentSize());
  }

  @Test
  public void getStoredDate() {
    metadata.addNode("/", new Group("_bdb"));
    metadata.addNode("/_bdb", new Attribute("stored_date", "20111213"));

    assertEquals(new Date(2011, 12, 13), classUnderTest.getStoredDate());
  }

  @Test
  public void getStoredTime() {
    metadata.addNode("/", new Group("_bdb"));
    metadata.addNode("/_bdb", new Attribute("stored_time", "141516"));

    assertEquals(new Time(14, 15, 16), classUnderTest.getStoredTime());
  }

  @Test
  public void getContentStream() {
    String uuidStr = "00000000-0000-0000-0004-000000000001";
    metadata.addNode("/", new Group("_bdb"));
    metadata.addNode("/_bdb", new Attribute("uuid", uuidStr));
    InputStream mockStream = createMock(InputStream.class);
    
    expect(database.getFileContent(UUID.fromString(uuidStr)))
      .andReturn(mockStream);
    replayAll();

    InputStream result = classUnderTest.getContentStream();
    assertSame(mockStream, result);
    verifyAll();
  }
}
