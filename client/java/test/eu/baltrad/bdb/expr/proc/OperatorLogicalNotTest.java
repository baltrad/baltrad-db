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
import eu.baltrad.bdb.expr.EvaluationError;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.StringExpression;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class OperatorLogicalNotTest {
  OperatorLogicalNot classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new OperatorLogicalNot();
  }

  @Test
  public void executeUnary() {
    Expression expr = new BooleanExpression(true);

    Expression result = classUnderTest.executeUnary(expr);
    assertTrue(result.isBoolean());
    assertEquals(Boolean.valueOf(false), result.toBoolean());
  }

  @Test
  public void executeUnary_invalidType() {
    Expression expr = new StringExpression("qwe");
    try {
      classUnderTest.executeUnary(expr);
      fail("expected EvaluationError");
    } catch (EvaluationError e) { }

  }
}
