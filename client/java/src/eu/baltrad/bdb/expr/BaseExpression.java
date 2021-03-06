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

import java.util.Iterator;
import java.util.NoSuchElementException;

public abstract class BaseExpression implements Expression {
  private class NullExpressionIterator implements Iterator<Expression> {
    public boolean hasNext() { return false; }
    public Expression next() {
      throw new NoSuchElementException();
    }

    public void remove() {
      throw new UnsupportedOperationException();
    }
  }
  
  @Override
  public boolean isList() {
    return getType() == Type.LIST;
  }
  
  @Override
  public boolean isNumber() {
    return isLong() || isDouble();
  }
  
  @Override
  public boolean isLong() {
    return getType() == Type.LONG;
  }
  
  @Override
  public boolean isDouble() {
    return getType() == Type.DOUBLE;
  }
  
  @Override
  public boolean isBoolean() {
    return getType() == Type.BOOLEAN;
  }
  
  @Override
  public boolean isString() {
    return getType() == Type.STRING;
  }

  @Override
  public boolean isSymbol() {
    return getType() == Type.SYMBOL;
  }

  @Override public Long toLong() { return null; }
  @Override public Double toDouble() { return null; }
  @Override public Boolean toBoolean() { return null; } // XXX: make abstract
  
  @Override
  public Expression popFirst() {
    throw new IndexOutOfBoundsException();
  }
  
  @Override
  public Expression get(int index) {
    throw new IndexOutOfBoundsException();
  }
  
  @Override
  public int size() { return 0; }
  
  @Override
  public Iterator<Expression> iterator() {
    return new NullExpressionIterator();
  }
}
