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

package eu.baltrad.bdb.expr;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

public class SymbolExpressionTest {
  private SymbolExpression classUnderTest;

  @Before
  public void setUp() {
    classUnderTest = new SymbolExpression("foo");
  }

  @Test
  public void equals_sameInstance() {
    assertTrue(classUnderTest.equals(classUnderTest));
  }

  @Test
  public void equals_otherType() {
    assertFalse(classUnderTest.equals(new String("foo")));
  }

  @Test
  public void equals_sameValue() {
    assertTrue(classUnderTest.equals(new SymbolExpression("foo")));
  }

  @Test
  public void equals_differentValue() {
    assertFalse(classUnderTest.equals(new SymbolExpression("bar")));
  }

  @Test
  public void hashCode_sameValue() {
    SymbolExpression other = new SymbolExpression("foo");
    assertEquals(classUnderTest.hashCode(), other.hashCode());
  }
}
