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

import eu.baltrad.bdb.expr.Expression;
import eu.baltrad.bdb.expr.ExpressionFactory;
import eu.baltrad.bdb.expr.ListExpression;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import static org.easymock.EasyMock.*;
import org.easymock.EasyMockSupport;

public class BinaryOperatorTest extends EasyMockSupport {
  BinaryOperator classUnderTestMock;
  BinaryOperator classUnderTest;

  @Before
  public void setUp() {
    classUnderTestMock = createMock(BinaryOperator.class);
    classUnderTest = new BinaryOperator() {
      @Override
      protected Expression executeBinary(Expression lhs, Expression rhs) {
        return classUnderTestMock.executeBinary(lhs, rhs);
      }
    };
    resetAll();
  }

  @Test
  public void execute() {
    Expression lhs = createMock(Expression.class);
    Expression rhs = createMock(Expression.class);
    Expression expected = createMock(Expression.class);

    ListExpression args = new ListExpression();
    args.add(lhs);
    args.add(rhs);

    expect(classUnderTestMock.executeBinary(lhs, rhs))
      .andReturn(expected);
    replayAll();

    Expression result = classUnderTest.execute(args);
    assertSame(expected, result);

    verifyAll();
  }

  @Test
  public void execute_tooFewArguments() {
    ListExpression args = new ListExpression();
    try {
      classUnderTest.execute(args);
      fail("expected EvaluationError");
    } catch (EvaluationError e) { }
  }

  @Test
  public void execute_tooManyArguments() {
    ListExpression args = new ListExpression();
    args.add(createMock(Expression.class));
    args.add(createMock(Expression.class));
    args.add(createMock(Expression.class));
    try {
      classUnderTest.execute(args);
      fail("expected EvaluationError");
    } catch (EvaluationError e) { }
  }
}
