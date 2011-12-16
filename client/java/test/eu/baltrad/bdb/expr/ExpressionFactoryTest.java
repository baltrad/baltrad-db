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

import java.util.*;

public class ExpressionFactoryTest {
  ExpressionFactory classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new ExpressionFactory();
  }

  @Test
  public void and_ExpressionList() {
    List<Expression> input = new ArrayList<Expression>();
    input.add(classUnderTest.literal(1));
    input.add(classUnderTest.literal(2));
    input.add(classUnderTest.literal(3));

    Expression expected = classUnderTest.and(
      classUnderTest.and(
        classUnderTest.literal(1),
        classUnderTest.literal(2)
      ),
      classUnderTest.literal(3)
    );

    Expression result = classUnderTest.and(input);
    assertEquals(expected, result);
  }

  @Test
  public void and_ExpressionConnection_OneElement() {
    List<Expression> input = new ArrayList<Expression>();
    input.add(classUnderTest.literal(1));
    
    Expression result = classUnderTest.and(input);
    assertEquals(classUnderTest.literal(1), result);
  }

  @Test
  public void and_ExpressionList_empty() {
    List<Expression> emptyList = Collections.emptyList();
    try {
      classUnderTest.and(emptyList);
      fail("expected IllegalArgumentException");
    } catch (IllegalArgumentException e) { }
  }

  @Test
  public void or_ExpressionList() {
    List<Expression> input = new ArrayList<Expression>();
    input.add(classUnderTest.literal(1));
    input.add(classUnderTest.literal(2));
    input.add(classUnderTest.literal(3));

    Expression expected = classUnderTest.or(
      classUnderTest.or(
        classUnderTest.literal(1),
        classUnderTest.literal(2)
      ),
      classUnderTest.literal(3)
    );

    Expression result = classUnderTest.or(input);
    assertEquals(expected, result);
  }

  @Test
  public void or_ExpressionConnection_OneElement() {
    List<Expression> input = new ArrayList<Expression>();
    input.add(classUnderTest.literal(1));
    
    Expression result = classUnderTest.or(input);
    assertEquals(classUnderTest.literal(1), result);
  }


  @Test
  public void or_ExpressionList_empty() {
    List<Expression> emptyList = Collections.emptyList();
    try {
      classUnderTest.or(emptyList);
      fail("expected IllegalArgumentException");
    } catch (IllegalArgumentException e) { }
  }
}
