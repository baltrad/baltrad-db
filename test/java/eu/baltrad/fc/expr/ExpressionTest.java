/*
Copyright 2010 Estonian Meteorological and Hydrological Institute

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
package eu.baltrad.fc.expr;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

public class ExpressionTest extends TestCase {
  private ExpressionFactory xpr = new ExpressionFactory();

  private Attribute attr;
  private Literal[] valueArray;
  private List<Literal> valueList;
  private ExpressionList valueElist;

  public void setUp() {
    attr = xpr.attribute("what/object");
    valueArray = new Literal[]{xpr.int64_(1), xpr.int64_(2)};

    valueList = new ArrayList<Literal>();
    valueList.add(xpr.int64_(1));
    valueList.add(xpr.int64_(2));
    
    valueElist = new ExpressionList();
    valueElist.append(xpr.int64_(1));
    valueElist.append(xpr.int64_(2));
  }

  public void testIn_Collection() {
    BinaryOperator op = attr.in(valueList);
    assertEquals("IN", op.op());
    assertTrue(op.lhs().equals(attr));
    assertTrue(op.rhs().equals(valueElist));
  }

  public void testIn_Array() {
    BinaryOperator op = attr.in(valueArray);
    assertEquals("IN", op.op());
    assertTrue(op.lhs().equals(attr));
    assertTrue(op.rhs().equals(valueElist));
  }

  public void testNotIn_Collection() {
    BinaryOperator op = attr.not_in(valueList);
    assertEquals("NOT IN", op.op());
    assertTrue(op.lhs().equals(attr));
    assertTrue(op.rhs().equals(valueElist));
  }

  public void testNotIn_Array() {
    BinaryOperator op = attr.not_in(valueArray);
    assertEquals("NOT IN", op.op());
    assertTrue(op.lhs().equals(attr));
    assertTrue(op.rhs().equals(valueElist));
  }

}
