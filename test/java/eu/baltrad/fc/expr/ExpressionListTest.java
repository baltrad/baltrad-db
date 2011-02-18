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

public class ExpressionListTest extends TestCase {
  private ExpressionFactory xpr = new ExpressionFactory();

  public void testConstruct_Collection() {
    List<Literal> values = new ArrayList<Literal>();
    values.add(xpr.int64_(1));
    values.add(xpr.int64_(2));
    
    ExpressionList l = new ExpressionList(values);
    assertEquals(2, l.size());
    assertTrue(l.at(0).equals(values.get(0)));
    assertTrue(l.at(1).equals(values.get(1)));
  }

  public void testConstruct_Array() {
    Literal[] values = new Literal[]{xpr.int64_(1), xpr.int64_(2)};

    ExpressionList l = new ExpressionList(values);
    assertEquals(2, l.size());
    assertTrue(l.at(0).equals(values[0]));
    assertTrue(l.at(1).equals(values[1]));
  }
}
