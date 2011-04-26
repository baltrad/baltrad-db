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
package eu.baltrad.fc.expr;

import java.util.ArrayList;
import java.util.List;

import junit.framework.TestCase;

public class ExpressionTest extends TestCase {
  private ExpressionFactory xpr;

  public void setUp() {
    xpr = new ExpressionFactory();
  }

  public void testEquals() {
    assertTrue((new Expression()).equals(new Expression()));
  }

  public void testConstructFromArray() {
    Expression[] a = new Expression[]{xpr.int64_(1), xpr.int64_(2)};
    Expression e = new Expression();
    e.push_back(xpr.int64_(1));
    e.push_back(xpr.int64_(2));

    Expression r = new Expression(a);
    assertTrue(r.equals(e));
  }

  public void testConstructFromCollection() {
    List<Expression> l = new ArrayList<Expression>();
    l.add(xpr.int64_(1));
    l.add(xpr.int64_(2));
    Expression e = new Expression();
    e.push_back(xpr.int64_(1));
    e.push_back(xpr.int64_(2));

    Expression r = new Expression(l);
    assertEquals(2, r.size());
    assertTrue(r.equals(e));
  }
}
