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
import eu.baltrad.bdb.expr.ListExpression;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class OperatorInTest {
  OperatorIn classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new OperatorIn();
  }

  @Test
  public void executeBinary() {
    Expression lhs = new StringExpression("qwe");
    ListExpression rhs = new ListExpression();
    rhs.add(new StringExpression("asd"));
    rhs.add(new StringExpression("qwe"));

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isBoolean());
    assertTrue(result.toBoolean());
  }

  @Test
  public void executeBinary_notFound() {
    Expression lhs = new StringExpression("qwe");
    ListExpression rhs = new ListExpression();
    rhs.add(new StringExpression("foo"));
    rhs.add(new StringExpression("bar"));

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isBoolean());
    assertFalse(result.toBoolean());
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
