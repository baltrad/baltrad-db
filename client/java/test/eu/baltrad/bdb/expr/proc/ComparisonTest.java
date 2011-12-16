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
import eu.baltrad.bdb.expr.DoubleExpression;
import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.ExpressionFactory;
import eu.baltrad.bdb.expr.LongExpression;
import eu.baltrad.bdb.expr.ListExpression;
import eu.baltrad.bdb.expr.StringExpression;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class ComparisonTest {
  Comparison classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new Comparison(Comparison.Operator.LT);
  }

  @Test
  public void executeBinary_String_String() {
    Expression lhs = new StringExpression("qwe");
    Expression rhs = new StringExpression("asd");

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isBoolean());
    assertFalse(result.toBoolean());
  }

  @Test
  public void executeBinary_Long_Long() {
    Expression lhs = new LongExpression(1);
    Expression rhs = new LongExpression(2);
    
    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isBoolean());
    assertTrue(result.toBoolean());
  }

  @Test
  public void executeBinary_Double_Double() {
    Expression lhs = new DoubleExpression(1.2);
    Expression rhs = new DoubleExpression(1.8);

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isBoolean());
    assertTrue(result.toBoolean());
  }

  @Test
  public void executeBinary_Long_Double() {
    Expression lhs = new LongExpression(1);
    Expression rhs = new DoubleExpression(1.8);

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isBoolean());
    assertTrue(result.toBoolean());
  }

  @Test
  public void executeBinary_Double_Long() {
    Expression lhs = new DoubleExpression(1.8);
    Expression rhs = new LongExpression(1);

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isBoolean());
    assertFalse(result.toBoolean());
  }

  @Test
  public void executeBinary_Boolean_Boolean() {
    Expression lhs = new BooleanExpression(true);
    Expression rhs = new BooleanExpression(true);

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isBoolean());
    assertFalse(result.toBoolean());
  }

/*
  @Test
  public void executeBinary_List_List() {
    ListExpression lhs = new ListExpression();
    lhs.add(new LongExpression(1));
    ListExpression rhs = new ListExpression();
    rhs.add(new LongExpression(1));
    rhs.add(new LongExpression(2));

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertTrue(result.isBoolean());
    assertTrue(result.toBoolean());
  }
*/

  @Test
  public void executeBinary_unsupportedTypes() {
    Expression lhs = new StringExpression("qwe");
    Expression rhs = new DoubleExpression(1.2);
    try {
      classUnderTest.executeBinary(lhs, rhs);
      fail("expected EvaluationError");
    } catch (EvaluationError e) { }
  }

  @Test
  public void comparisonResultToBoolean_EQ() {
    classUnderTest = new Comparison(Comparison.Operator.EQ);
    assertFalse(classUnderTest.comparisonResultToBoolean(-1));
    assertTrue(classUnderTest.comparisonResultToBoolean(0));
    assertFalse(classUnderTest.comparisonResultToBoolean(1));
  }

  @Test
  public void comparisonResultToBoolean_NE() {
    classUnderTest = new Comparison(Comparison.Operator.NE);
    assertTrue(classUnderTest.comparisonResultToBoolean(-1));
    assertFalse(classUnderTest.comparisonResultToBoolean(0));
    assertTrue(classUnderTest.comparisonResultToBoolean(1));
  }

  @Test
  public void comparisonResultToBoolean_GT() {
    classUnderTest = new Comparison(Comparison.Operator.GT);
    assertFalse(classUnderTest.comparisonResultToBoolean(-1));
    assertFalse(classUnderTest.comparisonResultToBoolean(0));
    assertTrue(classUnderTest.comparisonResultToBoolean(1));
  }

  @Test
  public void comparisonResultToBoolean_GE() {
    classUnderTest = new Comparison(Comparison.Operator.GE);
    assertFalse(classUnderTest.comparisonResultToBoolean(-1));
    assertTrue(classUnderTest.comparisonResultToBoolean(0));
    assertTrue(classUnderTest.comparisonResultToBoolean(1));
  }

  @Test
  public void comparisonResultToBoolean_LT() {
    classUnderTest = new Comparison(Comparison.Operator.LT);
    assertTrue(classUnderTest.comparisonResultToBoolean(-1));
    assertFalse(classUnderTest.comparisonResultToBoolean(0));
    assertFalse(classUnderTest.comparisonResultToBoolean(1));
  }

  @Test
  public void comparisonResultToBoolean_LE() {
    classUnderTest = new Comparison(Comparison.Operator.LE);
    assertTrue(classUnderTest.comparisonResultToBoolean(-1));
    assertTrue(classUnderTest.comparisonResultToBoolean(0));
    assertFalse(classUnderTest.comparisonResultToBoolean(1));
  }

}
