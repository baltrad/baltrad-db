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
import eu.baltrad.bdb.expr.ListExpression;
import eu.baltrad.bdb.expr.LongExpression;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class BinaryListCartesianTest {
  BinaryListCartesian classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new BinaryListCartesian(new OperatorAdd());
  }
  
  @Test
  public void executeBinary_lhsList() {
    ListExpression lhs = new ListExpression(
      new LongExpression(1),
      new LongExpression(2)
    );
    Expression rhs = new LongExpression(1);

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertEquals(2, result.size());
    assertEquals(Long.valueOf(2), result.get(0).toLong());
    assertEquals(Long.valueOf(3), result.get(1).toLong());
  }

  @Test
  public void executeBinary_rhsList() {
    Expression lhs = new LongExpression(1);
    ListExpression rhs = new ListExpression(
      new LongExpression(1),
      new LongExpression(2)
    );

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertEquals(2, result.size());
    assertEquals(Long.valueOf(2), result.get(0).toLong());
    assertEquals(Long.valueOf(3), result.get(1).toLong());
  }

  @Test
  public void executeBinary_bothList() {
    ListExpression lhs = new ListExpression(
      new LongExpression(1),
      new LongExpression(2)
    );
    ListExpression rhs = new ListExpression(
      new LongExpression(3),
      new LongExpression(4)
    );
    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertEquals(4, result.size());
    assertEquals(Long.valueOf(4), result.get(0).toLong());
    assertEquals(Long.valueOf(5), result.get(1).toLong());
    assertEquals(Long.valueOf(5), result.get(2).toLong());
    assertEquals(Long.valueOf(6), result.get(3).toLong());
  }

  @Test
  public void executeBinary_neitherList() {
    Expression lhs = new LongExpression(1);
    Expression rhs = new LongExpression(2);

    Expression result = classUnderTest.executeBinary(lhs, rhs);
    assertEquals(Long.valueOf(3), result.toLong());
  }
}
