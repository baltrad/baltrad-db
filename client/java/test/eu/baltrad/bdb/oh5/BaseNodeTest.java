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

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class BaseNodeTest {
  class FakeNode extends BaseNode {
    public FakeNode(String name) {
      super(name);
    }
  }

  FakeNode classUnderTest;
  
  @Before
  public void setUp() {
    classUnderTest = new FakeNode("");
  }

  @Test
  public void addChild() {
    Node node = new FakeNode("child");
    classUnderTest.addChild(node);
    assertSame(classUnderTest, node.getParent());
  }

  @Test
  public void addChild_duplicate() {
    Node node1 = new FakeNode("child");
    Node node2 = new FakeNode("child");

    classUnderTest.addChild(node1);
    classUnderTest.addChild(node2);
    assertNull(node1.getParent());
    assertSame(classUnderTest, node2.getParent());
    assertSame(node2, classUnderTest.getChild("child"));
  }

  @Test
  public void getChild() {
    Node node = new FakeNode("child");
    classUnderTest.addChild(node);
    assertSame(node, classUnderTest.getChild("child"));
  }

  @Test
  public void getChild_missing() {
    assertNull(classUnderTest.getChild("foo"));
  }

  @Test
  public void getPath_noParent() {
    assertEquals("/", classUnderTest.getPath());
  }

  @Test
  public void getPath() {
    Node child = new FakeNode("child");
    classUnderTest.addChild(child);
    assertEquals("/child", child.getPath());

  }

}
