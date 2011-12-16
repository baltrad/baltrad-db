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

import java.util.Collections;
import java.util.List;
import java.util.ArrayList;

public class NodeIteratorTest extends EasyMockSupport {
  Node root;
  NodeIterator classUnderTest;

  @Before
  public void setUp() {
    root = createMock(Node.class);
    classUnderTest = new NodeIterator(root);
  }
  
  @Test
  public void next() {
    Node n1 = createMock(Node.class);
    Node n2 = createMock(Node.class);
    Node n3 = createMock(Node.class);
    List<Node> noChildren = Collections.emptyList();
    List<Node> rootChildren = new ArrayList<Node>();
    rootChildren.add(n1);
    rootChildren.add(n2);
    List<Node> n1Children = new ArrayList<Node>();
    n1Children.add(n3);

    expect(root.getChildren())
      .andReturn(rootChildren);
    expect(n1.getChildren())
      .andReturn(n1Children);
    expect(n2.getChildren())
      .andReturn(noChildren);
    expect(n3.getChildren())
      .andReturn(noChildren);
    replayAll();
    
    assertEquals(root, classUnderTest.next());
    assertEquals(n1, classUnderTest.next());
    assertEquals(n2, classUnderTest.next());
    assertEquals(n3, classUnderTest.next());
    assertFalse(classUnderTest.hasNext());
    verifyAll();
  }
}
