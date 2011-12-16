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

import eu.baltrad.bdb.expr.DoubleExpression;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.ExpressionFactory;
import eu.baltrad.bdb.expr.LongExpression;
import eu.baltrad.bdb.expr.StringExpression;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class OperatorMulTest {
  OperatorMul classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new OperatorMul();
  }

  @Test
  public void executeBinary_Long_Long() {
    Expression lhs = new LongExpression(2);
    Expression rhs = new LongExpression(3);
    
    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isLong());
    assertEquals(new Long(6), result.toLong());
  }

  @Test
  public void executeBinary_Double_Double() {
    Expression lhs = new DoubleExpression(3.0);
    Expression rhs = new DoubleExpression(2.0);

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isDouble());
    assertEquals(new Double(6.0), result.toDouble());
  }

  @Test
  public void executeBinary_Long_Double() {
    Expression lhs = new LongExpression(2);
    Expression rhs = new DoubleExpression(1.5);

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isDouble());
    assertEquals(new Double(3.0), result.toDouble());
  }

  @Test
  public void executeBinary_Double_Long() {
    Expression lhs = new DoubleExpression(1.8);
    Expression rhs = new LongExpression(2);

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isDouble());
    assertEquals(new Double(3.6), result.toDouble());
  }

  @Test
  public void executeBinary_unsupportedTypes() {
    Expression lhs = new StringExpression("qwe");
    Expression rhs = new DoubleExpression(1.2);
    try {
      classUnderTest.executeBinary(lhs, rhs);
      fail("expected EvaluationError");
    } catch (EvaluationError e) { }
  }
}
