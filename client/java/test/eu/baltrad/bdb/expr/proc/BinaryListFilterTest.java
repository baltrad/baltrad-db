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

public class BinaryListFilterTest {
  BinaryOperator binOp;
  BinaryListFilter classUnderTest;

  @Before
  public void setUp() {
    //binOp = createMock(BinaryOperator.class);
    classUnderTest = new BinaryListFilter(binOp);
  }

  @Test
  public void test() {

  }
}
