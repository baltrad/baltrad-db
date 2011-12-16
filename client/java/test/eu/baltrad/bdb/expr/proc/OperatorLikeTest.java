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

package eu.baltrad.bdb.expr.proc;

import eu.baltrad.bdb.expr.BooleanExpression;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.StringExpression;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class OperatorLikeTest {
  OperatorLike classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new OperatorLike();
  }

  private boolean matches(String input, String pattern) {
    Expression lhs = new StringExpression(input);
    Expression rhs = new StringExpression(pattern);

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isBoolean());
    return result.toBoolean();
  }

  @Test
  public void executeBinary_plain() {
    assertFalse(matches("foo", "bar"));
    assertTrue(matches("foo", "foo"));
  }
  
  @Test
  public void executeBinary_star() {
    String input = "foobarbaz";

    assertTrue(matches(input, "foo*"));
    assertTrue(matches(input, "*bar*"));
    assertTrue(matches(input, "*baz"));
    assertFalse(matches(input, "bar*"));
    assertFalse(matches(input, "*bab*"));
    assertFalse(matches(input, "*bab"));
  }

  @Test
  public void executeBinary_qmark() {
    String input = "foobar";

    assertTrue(matches(input, "f?o??r"));
    assertFalse(matches(input, "foob?"));
    assertFalse(matches(input, "foobar?"));
    assertFalse(matches(input, "?foob??"));
  }

  @Test
  public void executeBinary_invalidTypes() {
    Expression lhs = new StringExpression("qwe");
    Expression rhs = new BooleanExpression(true);
    try {
      classUnderTest.executeBinary(lhs, rhs);
      fail("expected EvaluationError");
    } catch (EvaluationError e) { }
  }
}
