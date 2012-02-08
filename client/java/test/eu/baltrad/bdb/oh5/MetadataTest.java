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

package eu.baltrad.bdb.oh5;

import static org.easymock.EasyMock.*;
import org.easymock.EasyMockSupport;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class MetadataTest extends EasyMockSupport {
  Node root;
  Metadata classUnderTest;
  
  @Before
  public void setUp() {
    root = createMock(Node.class);
    classUnderTest = new Metadata(root);
    resetAll();
  }

  @Test
  public void addNode() {
    Node node1 = createMock(Node.class);
    Node node2 = createMock(Node.class);

    expect(root.getChild("node1"))
      .andReturn(node1);
    node1.addChild(node2);
    replayAll();

    classUnderTest.addNode("/node1", node2);
    verifyAll();
  }

  @Test
  public void addNode_root() {
    Node node = createMock(Node.class);

    root.addChild(node);
    replayAll();

    classUnderTest.addNode("/", node);
    verifyAll();
  }

  @Test
  public void addNode_missingParent() {
    Node node = createMock(Node.class);

    expect(root.getChild("foo"))
      .andReturn(null);
    replayAll();

    try {
      classUnderTest.addNode("/foo", node);
      fail("expected RuntimeException");
    } catch (RuntimeException e) { }
    verifyAll();
  }

  @Test
  public void getNode() {
    Node node1 = createMock(Node.class);
    Node node2 = createMock(Node.class);

    expect(root.getChild("node1"))
      .andReturn(node1);
    expect(node1.getChild("node2"))
      .andReturn(node2);
    replayAll();

    assertSame(node2, classUnderTest.getNode("/node1/node2"));
    verifyAll();
  }

  @Test
  public void getNode_missing() {
    expect(root.getChild("qwe"))
      .andReturn(null);
    replayAll();

    assertNull(classUnderTest.getNode("/qwe"));
    verifyAll();
  }

  @Test
  public void getNode_root() {
    assertSame(root, classUnderTest.getNode("/"));
  }

  @Test
  public void getSource() {
    classUnderTest = new Metadata();
    classUnderTest.addNode("/", new Group("what"));
    classUnderTest.addNode("/what", new Attribute("source", "WMO:12345"));

    Source source = classUnderTest.getSource();
    assertNotNull(source);
    assertEquals("12345", source.get("WMO"));
  }

  @Test
  public void getSource_noAttribute() {
    classUnderTest = new Metadata();
    assertNull(classUnderTest.getSource());
  }

  @Test
  public void getBdbSource_noAttribute() {
    classUnderTest = new Metadata();
    assertNull(classUnderTest.getBdbSource());
  }

  @Test
  public void getWhatSource() {
    classUnderTest = new Metadata();
    classUnderTest.addNode("/", new Group("what"));
    classUnderTest.addNode("/what", new Attribute("source", "WMO:12345"));    

    assertEquals("WMO:12345", classUnderTest.getWhatSource());
  }

  @Test
  public void getWhatSource_noAttribute() {
    classUnderTest = new Metadata();
    assertNull(classUnderTest.getWhatSource());
  }
}
