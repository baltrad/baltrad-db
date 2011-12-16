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

package eu.baltrad.bdb.expr;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import java.util.ArrayList;
import java.util.List;

public class ListExpressionTest {
  ListExpression classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new ListExpression();
  }

  @Test
  public void contructFromArray() {
    classUnderTest = new ListExpression(
      new LongExpression(1),
      new LongExpression(2)
    );

    assertEquals(2, classUnderTest.size());
    assertEquals(new Long(1), classUnderTest.get(0).toLong());
    assertEquals(new Long(2), classUnderTest.get(1).toLong());
  }

  @Test
  public void size() {
    assertEquals(0, classUnderTest.size());
    classUnderTest.add(new LongExpression(1));
    assertEquals(1, classUnderTest.size());
  }

  @Test
  public void iteration() {
    List<Long> expected = new ArrayList<Long>();
    expected.add(new Long(1));
    expected.add(new Long(2));
    Expression v1 = new LongExpression(1);
    Expression v2 = new LongExpression(2);
    classUnderTest.add(v1);
    classUnderTest.add(v2);
    
    int iterations = 0;
    for (Expression expr : classUnderTest) {
      assertEquals(expr.toLong(), expected.remove(0));
      iterations++;
    }
    assertEquals(2, iterations);
  }

  @Test
  public void toBoolean() { 
    assertFalse(classUnderTest.toBoolean());
    classUnderTest.add(new LongExpression(1));
    assertTrue(classUnderTest.toBoolean());
  }

  @Test
  public void equals_sameInstance() {
    assertTrue(classUnderTest.equals(classUnderTest));
  }

  @Test
  public void equals_empty() {
    ListExpression other = new ListExpression();
    assertTrue(classUnderTest.equals(other));
  }

  @Test
  public void equals_notEmpty() {
    ListExpression other = new ListExpression();
    other.add(new LongExpression(1));
    classUnderTest.add(new LongExpression(1));
    assertTrue(classUnderTest.equals(other));
  }

  @Test
  public void equals_notEmpty_differentValue() {
    ListExpression other = new ListExpression();
    other.add(new LongExpression(1));
    classUnderTest.add(new LongExpression(2));
    assertFalse(classUnderTest.equals(other));
  }

  @Test
  public void equals_differentType() {
    classUnderTest.add(new LongExpression(1));
    assertFalse(classUnderTest.equals(new LongExpression(1)));
  }
  
  @Test
  public void hashCode_sameValue() {
    ListExpression other = new ListExpression();
    assertEquals(classUnderTest.hashCode(), other.hashCode());
  }
}
