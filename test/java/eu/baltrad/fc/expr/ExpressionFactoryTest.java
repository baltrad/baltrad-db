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

public class ExpressionFactoryTest extends TestCase {
  private ExpressionFactory xpr = new ExpressionFactory();

  public void testAnd_Array() {
    Literal[] values = new Literal[]{xpr.long_(1), xpr.long_(2)};
    Expression expected = xpr.long_(1).and_(xpr.long_(2));

    assertTrue(xpr.and_(values).equals(expected));
  }

  public void testAnd_Collection() {
    List<Literal> values = new ArrayList<Literal>();
    values.add(xpr.long_(1));
    values.add(xpr.long_(2));

    Expression expected = xpr.long_(1).and_(xpr.long_(2));

    assertTrue(xpr.and_(values).equals(expected));
  }

  public void testOr_Array() {
    Literal[] values = new Literal[]{xpr.long_(1), xpr.long_(2)};
    Expression expected = xpr.long_(1).or_(xpr.long_(2));

    assertTrue(xpr.or_(values).equals(expected));
  }

  public void testOr_Collection() {
    List<Literal> values = new ArrayList<Literal>();
    values.add(xpr.long_(1));
    values.add(xpr.long_(2));

    Expression expected = xpr.long_(1).or_(xpr.long_(2));

    assertTrue(xpr.or_(values).equals(expected));
  }
}
